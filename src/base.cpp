//
// Created by moss on 2/12/22.
//

#include "base.h"

const mycelium::type mycelium::type::pointer(0, "pointer", 8);
const mycelium::type mycelium::type::integer(1, "int", 8);
const mycelium::type mycelium::type::character(2, "character", 1);
const mycelium::type mycelium::type::string(3, "string", 8);
const mycelium::type mycelium::type::list(4, "list", 8);
const mycelium::type mycelium::type::tuple(5, "tuple", 8);
const mycelium::type mycelium::type::oper(6, "operator", 8);
const mycelium::type mycelium::type::array(7, "array", 8);
const mycelium::type mycelium::type::cstring(8, "twine", 8);
const mycelium::type mycelium::type::token(9, "token", 8);
const mycelium::type mycelium::type::none(10, "none", 0);

std::vector<mycelium::type> mycelium::type::types = {};

std::vector<std::string> mycelium::type::strings = {};

std::vector<std::string> mycelium::token::whitespace_strings = {};
std::vector<std::string> mycelium::token::grouping_strings = {};
std::vector<std::string> mycelium::token::keyword_strings = {};
std::vector<std::string> mycelium::token::line_end = {};
std::vector<std::string> mycelium::token::seperator_strings = {};
std::vector<std::string> mycelium::token::oper_strings = {};
std::vector<std::vector<std::string>> mycelium::token::string_lists = {};
std::vector<std::string> mycelium::token::strings = {};
std::vector<std::string> mycelium::token::type_names = {};
std::string mycelium::token::line_comment = "//";
std::string mycelium::token::open_block_comment = "/*";
std::string mycelium::token::close_block_comment = "*/";
std::string mycelium::token::function_keyword = "func";
std::string mycelium::token::operator_keyword = "oper";
std::string mycelium::token::conditional_keyword = "cond";
std::string mycelium::token::class_keyword = "class";


void mycelium::throw_error(const std::string& error, int code) {
	std::cerr << "ERROR: " << error << std::endl;
	exit(code);
}


void mycelium::initialize_static_values() {
	for (auto& type : {mycelium::type::pointer, mycelium::type::integer, mycelium::type::character, mycelium::type::string, mycelium::type::list, mycelium::type::tuple, mycelium::type::oper, mycelium::type::array, mycelium::type::cstring, mycelium::type::token, mycelium::type::none}) {
		type::types.push_back(type);
	}

	for (auto& type : type::types) {
		type::strings.push_back(type.name);
	}

	if (show_debug_lines) {
		std::cout << type::strings << std::endl;
	}

	token::whitespace_strings.insert(token::whitespace_strings.end(), {" ", "	"});
	token::grouping_strings.insert(token::grouping_strings.end(), {"(", ")", "{", "}", "[", "]", "<", ">"});
	token::keyword_strings.insert(token::keyword_strings.end(), {token::function_keyword, token::operator_keyword, token::conditional_keyword, token::class_keyword, "this"});
	token::line_end.insert(token::line_end.end(), {";\n", ";"});
	token::line_end.insert(token::line_end.end(), {","});
	token::oper_strings.insert(token::oper_strings.end(), {"+", "-", "*", "/", "%", "=", "==", "!=", "<=", ">=",
														   "&&", "||", "++", "--", "!", "<<", ">>", "&", "|", "~"});
	token::string_lists.insert(token::string_lists.end(), {token::oper_strings, token::whitespace_strings,
														   token::grouping_strings, token::keyword_strings, type::strings, token::line_end});

	token::type_names.insert(mycelium::token::type_names.end(), {"operator", "whitespace", "grouping", "keyword", "type", "endline", "word", "num",
													 "invalid", "newline"});

	if (show_debug_lines) {
		std::cout << token::string_lists << std::endl;
	}

	for (const auto& token_string_list : token::string_lists) {
		for (const auto& token_string : token_string_list) {
			token::strings.push_back(token_string);
		}
	}
}

template <typename T>
bool mycelium::vector_contains(const std::vector<T>& vec, const T& con) {
	for (const auto &v: vec) {
		if (v == con) return true;
	}
	return false;
}

template <typename T>
std::ostream& mycelium::operator<<(std::ostream& os, const std::vector<T>& dt) {
	if (dt.empty()) {
		os << "{}";
		return os;
	}

	int i = 0;

	os << "{ ";

	for (; i < dt.size() - 1; i++) {
		if (std::is_same<T, std::string>::value) {
			os << "\"" << dt[i] << "\"" << ", ";
		}
		else {
			os << dt[i] << ", ";
		}
	}

	if (std::is_same<T, std::string>::value) {
		os << "\"" << dt[i] << "\"" << " }";
	}
	else {
		os << dt[i] << " }";
	}

	return os;
}

std::vector<std::string> mycelium::string_split(const std::string& str, const std::vector<std::string>& spl) {
	if (str.empty()) {
		return {};
	}

	std::vector<std::string> out;

	size_t pos;
	size_t prev_pos = 0;
	size_t min_pos;
	std::string split;

	while (prev_pos < str.length()) {

		min_pos = -1;

		for (auto &comp: spl) {
			if ((pos = str.find(comp, prev_pos)) != std::string::npos) {
				if (pos < min_pos || min_pos == -1) {
					min_pos = pos;
					split = comp;
				}
			}
		}

		if (min_pos == -1) {
			break;
		}

		if (min_pos != prev_pos) {
			out.push_back(str.substr(prev_pos, min_pos - prev_pos));
		}

		out.push_back(split);

		prev_pos = min_pos + split.length();
	}
	if (min_pos >= str.length()) {
		out.push_back(str.substr(prev_pos, min_pos - prev_pos));
	}

	return out;
}

mycelium::token_type mycelium::token::find_type(const std::string &string) {
	for (int type = 0; type < string_lists.size(); type++) {
		auto& token_string_list = string_lists[type];
		if (mycelium::vector_contains(token_string_list, string)) {
			return (token_type)type;
		}
	}

	bool is_num = true;

	for (char c: string) {
		if (!isdigit(c)) {
			if (show_debug_lines) {
				std::cout << c << " is not a digit" << std::endl;
			}
			is_num = false;
		}
		else {
			if (show_debug_lines) {
				std::cout << c << " is a digit" << std::endl;
			}
		}
	}

	if (is_num) {
		return num;
	} else if (isdigit(string.c_str()[0])) {
		return invalid;
	} else {
		return word;
	}
}

std::string mycelium::operatr::generate_name_from_context(std::vector<mycelium::token> &context) {
	std::string out;

	bool op_found = false;

	if (show_debug_lines) {
		std::cout << context.size() << std::endl;
	}

	for (int i = 0; i < context.size(); i++) {
		if (show_debug_lines) {
			std::cout << "i: " << i << "\ttoken: " << context[i].string << std::endl;
		}

		if (context[i].type == ttype) {
			out.append(context[i].string);
			i++;
		}
		else if (context[i].string == "this") {
			out.append(context[i].string);
		}
		else {
			if (op_found) {
				throw_error("operator definitions can only contain one operator", 42001);
			}

			if (show_debug_lines) {
				std::cout << "oper: " << context[i].string << std::endl;
			}

			context[i].type = op;

			out.append(encode_operator(context[i].string));
			if (!vector_contains(token::oper_strings, context[i].string)) {
				// Add operator to operator string list, so later we can easily just look for operators
				token::oper_strings.push_back(context[i].string);
			}
			op_found = true;
		}

		if (i < context.size()-1) {
			out.append("_");
		}
	}

	if (!op_found) {
		throw_error("no operator found in op definition", 41002);
	}

	return out;
}

std::string mycelium::operatr::encode_operator(const std::string &oper) {
	std::string out;

	if (show_debug_lines) {
		std::cout << oper << std::endl;
	}

	for (char c : oper) {
		out.append(int_to_hex_string(c));
	}

	if (show_debug_lines) {
		std::cout << out << std::endl;
	}

	return out;
}