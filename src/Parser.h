
#pragma once
#include <variant>

#include "Yggdrasil.h"

enum TokenType {
	weird,
    identifier,
    object,
    numeric_literal,
    arrow,
    keyword,
	newline,
	eof
};

struct Token
{
    TokenType type;
    std::variant<int, std::string> value;
};

class Lexer
{
public:
    std::string source_code;
    int position = 0;
    Token get_next_token();
	char current() { return source_code[position]; }
	bool is_digit() { return current() >= '0' && current() <= '9'; }
	bool is_char() { return current() >= 'a' && current() <= 'z' || current() == '_'; }
};

inline Token Lexer::get_next_token()
{
	if (position >= source_code.size()) return Token { eof, 0 };
	position++;
	
	while (current() == ' ') position++;
	if (current() == ';') while (current() != '\n') position++;
	if (current() == '\n') return { newline, 0 };
	if (current() == '>') return { arrow, 0 };
	
	if (is_digit()) {
		int value = 0;
		while (is_digit()) {
			value *= 10;
			value += current() - '0';
			position++;
		}
		position--;
		return { numeric_literal, value };
	}

	if (is_char()) {
		std::string id;
		while (is_char()) {
			id += current();
			position++;
		}
		if (id == "mk") return { keyword, 0 };
		if (id == "ct") return { keyword, 1 };
		if (id == "done") return { keyword, 2 };
		if (current() == '~') { position++; return { object, id }; };
		position--;
		return { identifier, id };
	}

	return Token { weird, 0 };
}
