
#pragma once

#include <variant>

#include "Yggdrasil.h"
#include "Graph.h"

namespace Yggdrasil {

enum TokenType {
	error,
    identifier,
    object,
    numeric_literal,
	string_literal,
    arrow,
	colon,
	open_brace,
	close_brace,
  	newline,
	comma,
	ampersand,
	plus,
	minus,
	slash,
	asterisk,
	eof
};

struct Token
{
    TokenType type;
    std::string value = "";
};

class Lexer
{
    int position = 0;
	char current();
	bool is_digit();
	bool is_char();

protected:
	Token get_next_token();
	virtual ~Lexer() = 0;
	uint line = 1;

public:
	std::string source_code;
};

class Parser : public Lexer
{
	Token current = { TokenType::error, "" };
	void error(std::string);
	void expect(TokenType);
	bool line_end();
	void parse_declaration(std::string);
	void parse_connection(std::string);

	float parse_expression();
	float parse_factor();
	float parse_product();

	std::string parse_parameter();

	Graph* program;
public:
	void parse_program(Graph&);
};

}

