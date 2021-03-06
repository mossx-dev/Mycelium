//
// Created by moss on 4/17/22.
//
#pragma once

#include <parser.h>
#include <base.h>
#include "run_test.h"

bool test_pattern_match() {
	std::vector<mycelium::token> tokens = {};

	bool test_passed = true;

	mycelium::initialize_static_values();

	tokens.emplace_back(mycelium::token_type::ttype, "int");
	tokens.emplace_back(mycelium::token_type::word, "var1");
	tokens.emplace_back(mycelium::token_type::op, "+");
	tokens.emplace_back(mycelium::token_type::ttype, "int");
	tokens.emplace_back(mycelium::token_type::word, "var2");

	mycelium::pattern_match pm(tokens);

	std::string test = "int var1 + int var2";
	mycelium::tokenizer tk({ test });

	tk.tokenize();

	if (!pm.is_match(tk.tokens)) {
		test_passed = false;
		print_failed();
		print_test_name("pattern match");
		std::cerr << test << std::endl;
	}
	else {
		print_success();
		print_test_name("pattern match");
		std::cout << test << std::endl;
	}


	test = "int a + int b";
	tk.lines = { test };

	tk.tokenize();

	if (!pm.is_match(tk.tokens)) {
		test_passed = false;
		print_failed();
		print_test_name("pattern match");
		std::cerr << test << std::endl;
	}
	else {
		print_success();
		print_test_name("pattern match");
		std::cout << test << std::endl;
	}


	test = "string a + string b";
	tk.lines = { test };

	tk.tokenize();

	if (pm.is_match(tk.tokens)) {
		test_passed = false;
		print_failed();
		print_test_name("pattern match");
		std::cerr << test << std::endl;
	}
	else {
		print_success();
		print_test_name("pattern match");
		std::cout << test << std::endl;
	}


	test = "string a + int b";
	tk.lines = { test };

	tk.tokenize();

	if (pm.is_match(tk.tokens)) {
		test_passed = false;
		print_failed();
		print_test_name("pattern match");
		std::cerr << test << std::endl;
	}
	else {
		print_success();
		print_test_name("pattern match");
		std::cout << test << std::endl;
	}

	return test_passed;
}