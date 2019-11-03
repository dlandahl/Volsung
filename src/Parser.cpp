
#include "Parser.h"
#include "Objects.h"

namespace Volsung {

Token Lexer::get_next_token()
{
	if (position >= source_code.size() - 1) return { eof, "" };
	position++;
	
	while (current() == ' ') position++;
	if (current() == ';') while (current() != '\n') position++;
	if (current() == '\n') {
		line++;
		return { newline, "" };
	}
	if (current() == '-') {
		position++;
		if (current() == '>') return { arrow, "" };
		position--;
		return { minus, "" };
	}
	if (current() == '{') return { open_brace, "" };
	if (current() == '}') return { close_brace, "" };
	if (current() == '(') return { open_paren, "" };
	if (current() == ')') return { close_paren, "" };
	if (current() == ':') return { colon, "" };
	if (current() == ',') return { comma, "" };
	if (current() == '&') return { ampersand, "" };
	if (current() == '*') return { asterisk, "" };
	if (current() == '+') return { plus, "" };
	if (current() == '/') return { slash, "" };
	
	if (is_digit()) {
		std::string value;
		value += current();
		position++;
		while (is_digit() || current() == '.') {
			value += current();
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
		if (current() == '~') return { object, id };
		position--;
		return { identifier, id };
	}

	if (current() == '"') {
		std::string string;
		position++;
		while (current() != '"') {
			string += current();
			position++;
		}
		return { string_literal, string };
	}
	
	log("Lexical Error");
	return Token { error, "" };
}

char Lexer::current()
{
	return source_code[position];
}

bool Lexer::is_digit()
{
	return current() >= '0' && current() <= '9';
}

bool Lexer::is_char()
{
	return current() >= 'a' && current() <= 'z' || current() == '_';
}

Lexer::~Lexer() {};




void Parser::parse_program(Graph& graph)
{
	program = &graph;
	program->reset();
	program->symbol_table["sf"] = { SAMPLE_RATE };
	program->symbol_table["tau"] = { 6.28318530f };
	while (current.type != eof) {
		current = get_next_token();

		if (line_end()) continue;
		else if (current.type == identifier) {
			std::string id = current.value;

			current = get_next_token();
			if (current.type == colon) parse_declaration(id);
			else if (current.type == open_brace) parse_connection(id);
		}
		else if (current.type == ampersand) {
			expect(identifier);
			std::string directive = current.value;
			std::vector<std::string> arguments;
			current = get_next_token();
			while (!line_end()) {
				arguments.push_back(current.value);
				current = get_next_token();
			}
			program->invoke_directive(directive, arguments);
		}
		else error("Expected declaration or connection, got " + debug_names[current.type]);
	}
}

void Parser::parse_declaration(std::string name)
{
	std::string object_name = name;

	current = get_next_token();
	if (current.type == numeric_literal || current.type == minus) {
		float value = parse_expression();
		program->symbol_table[name] = { value };
		return;
	} else if (current.type == string_literal) {
		std::string value = current.value;
		program->symbol_table[name] = { value };
		expect(newline);
		return;
	} else if (current.type != object) {
		error("RHS of declaration is " + debug_names[current.type] + ", should be string, object, or expression.");
		return;
	}
	
	std::string object_type = current.value;

	std::vector<std::string> arguments;
	current = get_next_token();
	if (!line_end()) {
		arguments.push_back(parse_parameter());
		while (!line_end()) {
			if (current.type != comma) error("Expected Comma or Newline, got " + debug_names[current.type]);
			current = get_next_token();
			arguments.push_back(parse_parameter());
		}
	}

	if (object_type == "osc") program->create_object<OscillatorObject>(object_name, arguments);
	else if (object_type == "add")   program->create_object<AddObject>(object_name, arguments);
	else if (object_type == "square")program->create_object<SquareObject>(object_name, arguments);
	else if (object_type == "delay") program->create_object<DelayObject>(object_name, arguments);
	else if (object_type == "mult")  program->create_object<MultObject>(object_name, arguments);
	else if (object_type == "sub")   program->create_object<SubtractionObject>(object_name, arguments);
	else if (object_type == "div")   program->create_object<DivisionObject>(object_name, arguments);
	else if (object_type == "noise") program->create_object<NoiseObject>(object_name, arguments);
	else if (object_type == "clock") program->create_object<ClockObject>(object_name, arguments);
	else if (object_type == "timer") program->create_object<TimerObject>(object_name, arguments);
	else if (object_type == "mod")   program->create_object<ModuloObject>(object_name, arguments);
	else if (object_type == "abs")   program->create_object<AbsoluteValueObject>(object_name, arguments);
	else if (object_type == "comp")  program->create_object<ComparatorObject>(object_name, arguments);
	else if (object_type == "filter")program->create_object<FilterObject>(object_name, arguments);
	else if (object_type == "file")  program->create_object<FileoutObject>(object_name, arguments);
	else error("No such object type: " + object_type);
}

std::string Parser::parse_parameter()
{
	if (current.type == identifier || current.type == minus) {
		if (!program->symbol_exists(current.value)) return "Unknown symbol";
		if (program->symbol_is_type<float>(current.value)) return std::to_string(parse_expression());
		if (program->symbol_is_type<std::string>(current.value)) {
			std::string argument = program->get_symbol_value_string(current.value);
			current = get_next_token();
			return argument;
		}
	}
	else if (current.type == numeric_literal) return std::to_string(parse_expression());
	else if (current.type == string_literal)
	{
		std::string argument = current.value;
		current = get_next_token();
		return argument;
	}
	else error("Invalid argument for declaration: " + debug_names[current.type] + ". Expected literal or identifier");
	return "Invalid";
}

void Parser::parse_connection(std::string name)
{
	std::string output_object = name;

	expect(numeric_literal);
	int output_index = std::stoi(current.value);
	expect(close_brace);
	
	expect(arrow);
	current = get_next_token();
	while (current.type != identifier)
	{
		TokenType operation = current.type;
		current = get_next_token();
		float value = parse_expression();
		std::string name = "inline_object" + std::to_string(inline_object_index++);
		std::vector<std::string> argument = { std::to_string(value) };

		switch (operation) {
			case (plus):     program->create_object<AddObject>(name, argument); break;
			case (minus):    program->create_object<SubtractionObject>(name, argument); break;
			case (asterisk): program->create_object<MultObject>(name, argument); break;
			case (slash):    program->create_object<DivisionObject>(name, argument); break;
			default: error("Invalid token for inline operation: " + debug_names[(TokenType) operation] + ". Expected arithmetic operator");
		}
		
		Program::connect_objects(*program, output_object, output_index, name, 0);
		output_index = 0;
		output_object = name;
		if (current.type != arrow) error("Expected arrow, got " + debug_names[current.type]);
		current = get_next_token();
	}
	std::string input_object = current.value;

	expect(open_brace);
	expect(numeric_literal);
	int input_index = std::stoi(current.value);
	expect(close_brace);
	
	Program::connect_objects(*program, output_object, output_index, input_object, input_index);
}

float Parser::parse_expression()
{
	float value = parse_product();
	while (current.type == plus || current.type == minus) {
		bool subtract = current.type == minus;
		current = get_next_token();
		if (subtract) value -= parse_product();
		else value += parse_product();
	}
	return value;
}

float Parser::parse_product()
{
	float value = parse_factor();
	while (current.type == asterisk || current.type == slash) {
		bool divide = current.type == slash;
		current = get_next_token();
		if (divide) value /= parse_factor();
		else value *= parse_factor();
	}
	return value;
}

float Parser::parse_factor()
{
	float value = 0;
	if (current.type == identifier) {
		if (program->symbol_exists(current.value) && program->symbol_is_type<float>(current.value))
			value = std::get<float>(program->symbol_table[current.value].value);
		else error("Symbol not found: " + current.value);
	}
	else if (current.type == numeric_literal) value = std::stof(current.value);
	else if (current.type == open_paren) {
		current = get_next_token();
		value = parse_expression();
		if (current.type != close_paren) error("Expected ')', got: " + debug_names[current.type]);
	}
	else if (current.type == minus) {
		current = get_next_token();
		return -parse_product();
	}
	else error("Couldn't get value of expression factor of type " + debug_names[current.type]);

	current = get_next_token();
	return value;
}

bool Parser::line_end()
{
	return (current.type == newline || current.type == eof);
}

void Parser::error(std::string error)
{
	log(std::to_string(line) + ": " + error);
}

void Parser::expect(TokenType expected)
{
	current = get_next_token();
	if (current.type != expected) error("Got " + debug_names[current.type] + ", expected " + debug_names[expected]);
}

}

