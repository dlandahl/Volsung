
#pragma once

#include <variant>
#include <exception>

#include "Volsung.hh"
#include "Graph.hh"

namespace Volsung {

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
	open_paren,
	close_paren,
	open_bracket,
	close_bracket,
  	newline,
	comma,
	ampersand,
	plus,
	minus,
	slash,
	asterisk,
	subscript,
	eof
};

inline std::map<TokenType, std::string> debug_names = {
{ error, "Unrecognised Token" },
{ identifier, "Identifier" },
{ object, "Object Type" },
{ numeric_literal, "Number" },
{ string_literal, "String" },
{ arrow, "Arrow" },
{ colon, "Colon" },
{ open_brace, "Open Brace" },
{ close_brace, "Close Brace" },
{ open_paren, "Open Parenthesis" },
{ close_paren, "Close Parenthesis" },
{ open_bracket, "Open Bracket" },
{ close_bracket, "Close Bracket" },
{ newline, "Newline" },
{ comma, "Comma" },
{ ampersand, "Ampersand" },
{ plus, "Plus" },
{ minus, "Minus" },
{ slash, "Slash" },
{ asterisk, "Asterisk" },
{ subscript, "Subscript" },
{ eof, "End of File" }
};

class ParseException : public std::exception
{
public:
	virtual const char* what() const noexcept override;
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
	bool peek(TokenType);
	virtual ~Lexer() = 0;
	uint line = 1;

public:
	std::string source_code;
};

class Parser : public Lexer
{
	Token current = { TokenType::error, "" };
	Token next_token();
	void error(std::string);
	void expect(TokenType);
	void verify(TokenType);
	bool line_end();
	void parse_declaration(std::string);
	void parse_connection(std::string);

	Sequence parse_sequence();

	TypedValue parse_expression();
	TypedValue parse_factor();
	TypedValue parse_product();

	int inline_object_index = 0;
	Graph* program;
public:
	void parse_program(Graph&);
};

}

