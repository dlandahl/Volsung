
#include "Yggdrasil.h"

enum class DataType {
    object
};

class Symbol
{
    DataType type;
};

std::unordered_map<std::string, Symbol> SymbolTable = { std::pair("Unkown Identifier") };

enum class TokenType {
    identifier,
    object,
    numeric_literal,
    index,
    arrow,
    keyword
};

using TokenValue = int;

class Token
{
    TokenType type;
    TokenValue value;
};

class Lexer
{
    std::string source_code;
    int position = 0;
    Token match_identifier();
};

inline Token Lexer::match_identifier()
{
    if (!is_digit(source_code[position])) return Token { TokenType::identifier, TokenValue(0) };
    int size = 0;
    while (is_digit(source_code[position]))
    {
        size++; position++;
    }
}
