
#include "Parser.h"
#include "Objects.h"

namespace Yggdrasil {

Token Lexer::get_next_token()
{
	if (position >= source_code.size() - 1) return { eof, 0 };
	position++;
	
	while (current() == ' ') position++;
	if (current() == ';') while (current() != '\n') position++;
	if (current() == '\n') {
		line++;
		return { newline, 0 };
	}
	if (current() == '-') {
		position++;
		if (current() == '>') return { arrow, 0 };
		else return { error, 0 };
	}
	if (current() == '(') return { open_paren, 0 };
	if (current() == ')') return { close_paren, 0 };
	
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
		if (id == "mk") return { keyword, mk };
		if (id == "ct") return { keyword, ct };
		if (current() == '~') { position++; return { object, id }; };
		position--;
		return { identifier, id };
	}
	
	log("Lexical Error");
	std::cout << int(current()) << std::endl;
	return Token { error, 0 };
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
		else if (current.type == keyword) {
			if (std::get<int>(current.value) == mk) parse_mk_command(graph);
			if (std::get<int>(current.value) == ct) parse_ct_command(graph);
		}
		else error("Expected declaration or connection" + std::to_string(current.type));
	}
}

void Parser::parse_mk_command(Graph& graph)
{
	expect(object);
	std::string object_type = std::get<std::string>(current.value);

	expect(identifier);
	std::string object_name = std::get<std::string>(current.value);

	std::string arguments;
	current = get_next_token();
	while (current.type != newline && current.type != eof) {
		if (current.type == identifier)
			arguments += std::get<std::string>(current.value);
			
		else if (current.type == numeric_literal)
			arguments += std::to_string(std::get<int>(current.value));
			
		else error("Invalid argument " + std::to_string(current.type));
		current = get_next_token();
	}

	arguments = "mk " + object_type + ' ' + object_name + ' ' + arguments;

	if (object_type == "osc") graph.create_object<OscillatorObject>(arguments);
	else if (object_type == "add")   graph.create_object<AddObject>(arguments);
	else if (object_type == "square")   graph.create_object<SquareObject>(arguments);
	else if (object_type == "delay") graph.create_object<DelayObject>(arguments);
	else if (object_type == "mult")  graph.create_object<MultObject>(arguments);
	else if (object_type == "sub")   graph.create_object<SubtractionObject>(arguments);
	else if (object_type == "div")   graph.create_object<DivisionObject>(arguments);
	else if (object_type == "noise") graph.create_object<NoiseObject>(arguments);
	else if (object_type == "clock") graph.create_object<ClockObject>(arguments);
	else if (object_type == "timer") graph.create_object<TimerObject>(arguments);
	else if (object_type == "mod")   graph.create_object<ModuloObject>(arguments);
	else if (object_type == "abs")   graph.create_object<AbsoluteValueObject>(arguments);
	else if (object_type == "comp")  graph.create_object<ComparatorObject>(arguments);
	else if (object_type == "filter")graph.create_object<FilterObject>(arguments);
	else if (object_type == "file")  graph.create_object<FileoutObject>(arguments);
	else error("No such object type");
}

void Parser::parse_ct_command(Graph& graph)
{
	expect(identifier);
	std::string output_object = std::get<std::string>(current.value);

	expect(open_paren);
	expect(numeric_literal);
	int output_index = std::get<int>(current.value);
	expect(close_paren);
	
	expect(arrow);
	expect(identifier);
	std::string input_object = std::get<std::string>(current.value);

	expect(open_paren);
	expect(numeric_literal);
	int input_index = std::get<int>(current.value);
	expect(close_paren);
	
	Program::connect_objects(graph, output_object, output_index, input_object, input_index);
}


}
