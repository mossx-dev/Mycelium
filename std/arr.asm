    ;; Author:  Moss Gallagher
    ;; Date:    14-Oct-21

%ifndef _mxs_std_arr_
%define _mxs_std_arr_

%include "std/exception.asm"
%include "std/type.asm"
%include "std/mem.asm"

    ;; Array archtecure
    ;; [memory size, user size, data type, data, data, etc...]
    ;;
    ;; Array vs. List
    ;; Similarities:
    ;;  Arrays and Lists are both consective byte of memory and store their sizes
    ;; Differences:
    ;;  Lists can have any type of element in any position where all elements in an arrays must have the same type.
    ;;  Lists can have multiple different types which store extra metadata for example 2d lists which store width and height.
    ;;  For these reasons array are both faster and more memory efficient but lists are generally easier to work with.

arr#meta#mem_size               equ 0            ; The size of the array in memory. This is different to user size because we're lying to the user
arr#meta#user_size              equ 8            ; The size of the array according to the user
arr#meta#type                   equ 16           ; The type of each element in the array. This is where lists and arrays differ.

arr#global_stride:              equ 100*8        ; The memory size of an array is always a multiple of this + the type specific metadata size
arr#meta_size:                  equ 3*8


; Args
;   rax: array pointer
;   rbx: data type
;   rcx: array size
; Returns
;   void
arr#populate_metadata:
    push    rsi
    push    rax
    push    r9
    mov     [rax+arr#meta#user_size], rcx               ; User size

    mov     [rax+arr#meta#type], rbx                    ; Array Type

    mov     r9, rax

    mov     rax, rbx
    call    type~sizeof

    mov     rax, rsi

    mul     rcx

    add     rax, arr#global_stride

    mov     [r9+arr#meta#mem_size], rax                ; Memory Size

    pop     r9
    pop     rax
    pop     rsi
    ret


; Args
;   rax: the type of the array
;   rbx: the size of the new array in elements
; Returns
;   rsi: pointer to the array
arr#new:
    push    rcx

    mov     rcx, rbx
    mov     rbx, rax

    mov     rax, arr#global_stride          ; Load the stride into rax

    add     rax, arr#meta_size              ; Add the metadata size
    call    mem~allocate                    ; Create an array in memory with the size specified in global stride

    lea     rax, [rsi]
    call    arr#populate_metadata

    pop     rcx
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~del:
    mov     rbx, [rax]
    call    mem~deallocate
    ret


; Args
;   rax: pointer to the array
; Returns
;   void
arr~print_string:
    .string:    db ", ", 0x0
arr~print:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    push    r11
    mov     r10, rax
    mov     r11, rbx
    mov     rdx, [r10+arr#meta#type]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10+arr#meta#user_size]          ; Get length
    mul     r9

    add     rcx, rax            ; Get the end address

    mov     rax, '['
    call    out~putc

    lea     r10, [r10+arr#meta_size]       ; Offset to account for size and type variables
    lea     rcx, [rcx+arr#meta_size]       ; Offset the end address too

    mov rax, [r10]
    mov rbx, rdx
    call type~print

    add     r10, r9            ; Increment the index

    .loop:
        mov     rax, arr~print_string
        mov     rbx, 2
        call    out~puts


        mov rax, [r10]
        mov rbx, rdx
        call type~print

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop


    mov     rax, ']'
    call    out~putc

    pop     r11
    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret

; Args
;   rax: pointer to the array
; Returns
;   void
arr~println:
    push    rax

    call    arr~print

    mov     rax, 0xA
    call    out~putc

    pop     rax
    ret


; Args
;   rax: pointer to the array
; Returns
;   void
arr~printn:
    push    rax
    push    rcx
    push    rdx
    push    rsi
    push    r9
    push    r10
    mov     r10, rax
    mov     rdx, [r10+arr#meta#type]        ; Get the type into rax
    mov     rax, rdx
    call    type~sizeof         ; Get the size of each element into rsi
    mov     r9, rsi             ; Move the size to a usable location

    lea     rcx, [r10]

    mov     rax, [r10+arr#meta#user_size]          ; Get length
    mul     r9

    add     rcx, rax            ; Get the end address



    lea     r10, [r10+arr#meta_size]       ; Offset by 16 to account for size and type variables
    lea     rcx, [rcx+arr#meta_size]       ; Offset the end address too

    .loop:
        mov rax, [r10]
        mov rbx, rdx
        call type~print

        mov     rax, 0xA
        call    out~putc

        add     r10, r9            ; Increment the index
    .loop_check:
        cmp     r10, rcx
        jl     .loop

    pop     r10
    pop     r9
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rax                 ; Perserve the pointer to the start of the array
    ret




; Args
;   rax: pointer to the array
; Returns
;   rsi: length of the array
arr~len:
    mov     rsi, [rax]
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
; Returns
;   rsi: element
arr~get:
    mov     rsi, [rax+arr#meta#user_size]
    cmp     rsi, rbx
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax
    push    rax
    mov     rax, [rax+arr#meta#type]        ; Get the type into rax
    call    type~sizeof         ; Get the size of each element into rsi

    mov     rax, rbx
    mul     rsi                 ; Get the byte index of the element
    mov     rsi, rax
    pop     rax
    add     rax, rsi            ; Get move the index minus 16

    mov     rsi, [rax+arr#meta_size]       ; Offset by the size of the metadata to account for size and type variables

    pop     rax                 ; Perserve the pointer to the start of the array
    ret


; Args
;   rax: pointer to the array
;   rbx: index of the element
;   rcx: write value
; Returns
;   void
arr~set:
    mov     rsi, [rax]          ; Get size
    cmp     rsi, rbx            ; Validate Index
    jg      .valid_index
    mov     rax, exception~runtime~bad_index
    call    exception~runtime~throw

    .valid_index:
    push    rax
    push    rax
    mov     rax, [rax+arr#meta#type]        ; Get the type into rax
    call    type~sizeof         ; Get the size of each element into rsi

    mov     rax, rbx
    mul     rsi                 ; Get the byte index of the element
    mov     rsi, rax
    pop     rax
    add     rax, rsi            ; Get move the index minus 16

    mov     [rax+16], rcx       ; Offset by 16 to account for size and type variables

    pop     rax                 ; Perserve the pointer to the start of the array
    ret



; Args
;   rax: pointer to the array
;   rbx: new size
; Returns
;   rax: new pointer if needed
arr~resize:
    push    r11
    push    r12
    push    rax
    mov     rcx, rbx
    mov     r12, rbx
    mov     rbx, [rax]
    mov     rax, [rax+arr#meta#type]        ; Get type
    call    type~sizeof
    mov     rax, rcx
    mul     rsi
    mov     rcx, rax
    mov     rax, rbx
    mul     rsi
    mov     rbx, rax
    pop     rax
    call    mem~reallocate
    mov     [rax], r12
    pop     r12
    pop     r11
    ret

; Args
;   rax: pointer to the array
;   rbx: the amount increase size by
; Returns
;   rax: new address pointer if needed
;   rsi: the new size
arr~increase_size:
    push    r11
    push    r12
    push    r13
    push    r14
    push    rax

    mov     r13, rbx            ; Store the increase amount
    lea     r12, [rax]          ; Store the address
    mov     r11, [rax+arr#meta#type]        ; Get the type

    mov     rax, r11
    call    type~sizeof         ; type.sizeof(rax)

    mov     rbx, rsi            ; rbx = size of each element
    mov     rax, [r12]          ; size
    mov     r14, rax
    add     rax, r13            ; rax = current_length + r13

    mov     rsi, rax

    mul     rbx                 ; rax = rax * rbx a.k.a the byte length of the array

    mov     rcx, rax

    mov     rax, r14

    mul     rbx

    mov     rbx, rax

    pop     rax
    push    rsi
    call    mem~reallocate
    mov     rbx, r14
    add     rbx, r13
    mov     [rax], rbx

    pop     rsi

    pop     r14
    pop     r13
    pop     r12
    pop     r11
    ret

; Args
;   rax: pointer to the array
;   rbx: value to push
; Returns
;   rax: the new pointer if needed
;
; Note: This function is not very performant. It is recommended that if you know how many elements you want to append you use resize then write each element after
arr~push:
    push    r9
    push    r10
    mov     r9, rbx             ; Store value to push in temp place
    mov     rbx, 1              ; Increase the size by 1
    call    arr~increase_size

    mov     rbx, rsi            ; Put the new size as the index for set
    sub     rbx, 1
    mov     rcx, r9             ; Put the value as the push value to write the end of the array
    call    arr~set             ; Set the value

    pop     r10
    pop     r9
    ret

; Args
;   rax: pointer to the array
; Returns
;   rsi: value
arr~pop:
    push    rbx
    push    rcx
    mov     rbx, [rax+arr#meta#user_size]          ; Load the size of the array into rbx

    sub     rbx, 1

    call    arr~get             ; rax.get(rbx)
    mov     [rax], rbx

    pop     rcx
    pop     rbx
    ret


%endif                          ; ifdef guard