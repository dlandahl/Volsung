
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
	greater_than,
	less_than,
  	newline,
	comma,
	ampersand,
	plus,
	minus,
	slash,
	asterisk,
	caret,
	many_to_one,
	one_to_many,
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
{ caret, "Caret" },
{ many_to_one, "Many-to-One" },
{ one_to_many, "One-to-Many" },
{ eof, "End of File" }
};

struct Token
{
    TokenType type;
    std::string value = "";
};

/*! \brief A lexical analyser for a Volsung program
 *  
 *  Instantiate instead a Parser, and use only the source_code field from this class.
 */
	 
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
	/*! \brief The source code to be lexed and parsed
	 *  
	 *  Set this to the code you wish to lex and parse.
	 */

	 std::string source_code;
};

/*! \brief A parser for a Volsung program
 *  
 *  Use this to parse a Volsung program, and populate the symbol tables of a Program class.
 */

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
	void make_object(std::string, std::string, std::vector<TypedValue>);
	Sequence parse_sequence();

	TypedValue parse_expression();
	TypedValue parse_product();
	TypedValue parse_power();
	TypedValue parse_factor();

	int inline_object_index = 0;
	Graph* program;
public:

	/*! \brief Parse a Program
	 *  
	 *  Parse the source code and supply the instance of Program to write the audio processing graph into.
	 */

	void parse_program(Graph&);
};

}

