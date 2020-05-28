
#pragma once

#include <tuple>
#include <variant>
#include <exception>

#include "VolsungCore.hh"
#include "Objects.hh"

namespace Volsung {

enum class TokenType {
    invalid,
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
    percent,
    backtick,
    plus,
    minus,
    slash,
    asterisk,
    caret,
    many_to_one,
    one_to_many,
    parallel,
    series,
    cross_connection,
    vertical_bar,
    elipsis,
    dot,
    eof
};

inline const std::map<TokenType, std::string> debug_names = {
    { TokenType::invalid, "unrecognised token" },
    { TokenType::identifier, "identifier" },
    { TokenType::object, "object type" },
    { TokenType::numeric_literal, "number" },
    { TokenType::string_literal, "string literal" },
    { TokenType::arrow, "'Arrow' (->)" },
    { TokenType::colon, "'Colon' (:)" },
    { TokenType::open_brace, "'Open Brace' ({)" },
    { TokenType::close_brace, "'Close Brace' (})" },
    { TokenType::open_paren, "'Open Parenthesis' (()" },
    { TokenType::close_paren, "'Close Parenthesis' ())" },
    { TokenType::open_bracket, "'Open Bracket' ([)" },
    { TokenType::close_bracket, "'Close Bracket' (])" },
    { TokenType::greater_than, "'Greater Than' (>)" },
    { TokenType::less_than, "'Less Than' (<)" },
    { TokenType::newline, "end-of-line" },
    { TokenType::comma, "'Comma' (,)" },
    { TokenType::ampersand, "'Ampersand' (&)" },
    { TokenType::percent, "'Percent' (%)" },
    { TokenType::backtick, "'Backtick' (`)" },
    { TokenType::plus, "'Plus' (+)" },
    { TokenType::minus, "'Minus' (-)" },
    { TokenType::slash, "'Slash' (/)" },
    { TokenType::asterisk, "'Asterisk' (*)" },
    { TokenType::caret, "'Caret' (^)" },
    { TokenType::many_to_one, "'Many-to-One' (>>)" },
    { TokenType::one_to_many, "'One-to-Many' (<>)" },
    { TokenType::vertical_bar, "'Vertical Bar' (|)" },
    { TokenType::parallel, "'Parallel Connection Operator' (=>)" },
    { TokenType::series, "'Series Connection Operator' (-->)" },
    { TokenType::cross_connection, "'Complete Bipartite Connection Operator' (x>)" },
    { TokenType::elipsis, "'Elipsis' (..)" },
    { TokenType::dot, "'Dot' (.)" },
    { TokenType::eof, "end-of-file" }
};

struct Token
{
    TokenType type;
    std::string value;
};

/*! \brief A lexical analyser for a Volsung program
 *  
 *  Instantiate instead a Parser, and use only the source_code field from this class.
 */

class Lexer
{
    bool is_digit() const;
    bool is_char() const;

protected:
    char current() const;
    Token get_next_token();
    bool peek(const TokenType);
    bool peek_expression();
    bool peek_connection();
    virtual ~Lexer() = 0;

    size_t line = 1;
    size_t position = (size_t) -1;

public: 
    /*! \brief The source code to be lexed and parsed
     *  
     *  Set this to the code you wish to lex and parse.
     */

     std::string source_code;
};

/*! \brief A parser for a Volsung program
 *  
 *  Use this to parse a Volsung program, and populate the symbol tables of a Program class instance.
 */

class Parser : public Lexer
{
    Token current_token = { TokenType::invalid, "" };
    Token next_token();
    void error(const std::string&) const;
    void expect(const TokenType);
    void verify(const TokenType) const;
    bool line_end() const;
    bool current_token_is(const TokenType) const;

    void parse_declaration();
    void parse_subgraph_declaration();
    void parse_connection();
    void parse_connection(std::string);

    Sequence parse_sequence();
    Number parse_number();
    void parse_directive();
    TypedValue parse_identifier();
    TypedValue parse_procedure_call(TypedValue, TypedValue = 0, bool = false);

    std::string get_object_to_connect();
    void make_object(const std::string&, const std::string&, const ArgumentList&);
    std::string parse_object_declaration(std::string = "");

    TypedValue parse_expression();
    TypedValue parse_sequence_generator();
    TypedValue parse_product();
    TypedValue parse_power();
    TypedValue parse_unary_postfix();
    TypedValue parse_backtick_operator();
    TypedValue parse_factor();

    int inline_object_index = 0;
    Graph* program = nullptr;

public:
    /*! \brief Parse a Program
     *  
     *  Parse the source code and supply the instance of Program to write the audio processing graph into.
     */

    bool parse_program(Graph&);
};

}

