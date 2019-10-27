
#pragma once

#include <variant>

#include "Yggdrasil.h"
#include "Graph.h"

namespace Yggdrasil {

enum TokenType {
	nothing,
    identifier,
    object,
    numeric_literal,
    arrow,
    keyword,
	newline,
	eof
};

enum Keyword {
	mk,
	ct,
};

struct Token
{
    TokenType type;
    std::variant<int, std::string> value;
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
	Token current = { nothing, 0 };
	void error(std::string);
	void expect(TokenType);
	void parse_mk_command(Graph&);
	void parse_ct_command(Graph&);
	
public:
	void parse_program(Graph&);
};

}
