
#include "Parser.h"
#include "Objects.h"

namespace Yggdrasil {

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
		else return { error, "" };
	}
	if (current() == '{') return { open_paren, "" };
	if (current() == '}') return { close_paren, "" };
	if (current() == ':') return { colon, "" };
	if (current() == ',') return { comma, "" };
	
	if (is_digit() || current() == '-') {
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
		if (current() == '~') { return { object, id }; };
		position--;
		return { identifier, id };
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





void Parser::error(std::string error)
{
	log(std::to_string(line) + ": " + error);
}

void Parser::expect(TokenType expected)
{
	current = get_next_token();
	if (current.type != expected) error("Expected something different " + std::to_string(expected));
}

void Parser::parse_program(Graph& graph)
{
	graph.reset();
	while (current.type != eof) {
		current = get_next_token();

		if (current.type == newline || current.type == eof) continue;
		else if (current.type == identifier) {
			std::string id = current.value;

			current = get_next_token();
			if (current.type == colon) parse_declaration(graph, id);
			if (current.type == open_paren) parse_connection(graph, id);
		}
		else error("Expected declaration or connection" + std::to_string(current.type));
	}
}

void Parser::parse_declaration(Graph& graph, std::string name)
{
	std::string object_name = name;

	expect(object);
	std::string object_type = current.value;

	std::vector<std::string> arguments;
	current = get_next_token();
	if (current.type != newline && current.type != eof) {
		arguments.push_back(current.value);
		current = get_next_token();

		while (current.type != newline && current.type != eof) {
			if (current.type != comma) error("Expected comma");
			current = get_next_token();
			arguments.push_back(current.value);
			current = get_next_token();
		}
	}

	if (object_type == "osc") graph.create_object<OscillatorObject>(object_name, arguments);
	else if (object_type == "add")   graph.create_object<AddObject>(object_name, arguments);
	else if (object_type == "square")   graph.create_object<SquareObject>(object_name, arguments);
	else if (object_type == "delay") graph.create_object<DelayObject>(object_name, arguments);
	else if (object_type == "mult")  graph.create_object<MultObject>(object_name, arguments);
	else if (object_type == "sub")   graph.create_object<SubtractionObject>(object_name, arguments);
	else if (object_type == "div")   graph.create_object<DivisionObject>(object_name, arguments);
	else if (object_type == "noise") graph.create_object<NoiseObject>(object_name, arguments);
	else if (object_type == "clock") graph.create_object<ClockObject>(object_name, arguments);
	else if (object_type == "timer") graph.create_object<TimerObject>(object_name, arguments);
	else if (object_type == "mod")   graph.create_object<ModuloObject>(object_name, arguments);
	else if (object_type == "abs")   graph.create_object<AbsoluteValueObject>(object_name, arguments);
	else if (object_type == "comp")  graph.create_object<ComparatorObject>(object_name, arguments);
	else if (object_type == "filter")graph.create_object<FilterObject>(object_name, arguments);
	else if (object_type == "file")  graph.create_object<FileoutObject>(object_name, arguments);
	else error("No such object type");
}

void Parser::parse_connection(Graph& graph, std::string name)
{
	std::string output_object = name;

	expect(numeric_literal);
	int output_index = std::stoi(current.value);
	expect(close_paren);
	
	expect(arrow);
	expect(identifier);
	std::string input_object = current.value;

	expect(open_paren);
	expect(numeric_literal);
	int input_index = stoi(current.value);
	expect(close_paren);
	
	Program::connect_objects(graph, output_object, output_index, input_object, input_index);
}


}

