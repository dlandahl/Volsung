
#include "Parser.hh"
#include "Objects.hh"

namespace Volsung {

const char* ParseException::what() const noexcept
{
	return "Volsung has encountered an error during parsing of the provided program and will exit the parsing stage.";
}

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
	if (current() == '[') return { open_bracket, "" };
	if (current() == ']') return { close_bracket, "" };
	if (current() == ':') return { colon, "" };
	if (current() == ',') return { comma, "" };
	if (current() == '&') return { ampersand, "" };
	if (current() == '*') return { asterisk, "" };
	if (current() == '+') return { plus, "" };
	if (current() == '/') return { slash, "" };
	if (current() == '_') return { subscript, "" };

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

bool Lexer::peek(TokenType expected)
{
	int old_position = position;
	int old_lines = line;
	bool ret;
	if (get_next_token().type == expected) ret = true;
	else ret = false;
	position = old_position;
	line = old_lines;
	return ret;
}

Lexer::~Lexer() {};




void Parser::parse_program(Graph& graph)
{
	program = &graph;
	program->reset();
	program->add_symbol("sf", SAMPLE_RATE);
	program->add_symbol("tau", TAU);
	try {

	while (current.type != eof) {
		next_token();

		if (line_end()) continue;
		else if (current.type == identifier) {
			std::string id = current.value;

			next_token();
			if (current.type == colon) parse_declaration(id);
			else if (current.type == open_brace) parse_connection(id);
			else error("Expected colon or open brace, got " + debug_names[current.type]);
		}
		else if (current.type == ampersand) {
			expect(identifier);
			std::string directive = current.value;
			std::vector<std::string> arguments;
			next_token();
			while (!line_end()) {
				arguments.push_back(current.value);
				next_token();
			}
			program->invoke_directive(directive, arguments);
		}
		else error("Expected declaration or connection, got " + debug_names[current.type]);
	}

	} catch (const ParseException& exception) {
		log(std::string(exception.what()));
	}
}

void Parser::parse_declaration(std::string name)
{
	std::string object_name = name;

	next_token();
	if (current.type == numeric_literal
		|| current.type == minus
		|| current.type == string_literal
		|| current.type == open_bracket
		|| current.type == open_paren) {
		TypedValue value = parse_expression();
		program->add_symbol(name, value);;
		return;
	} else if (current.type != object) {
		error("RHS of declaration is " + debug_names[current.type] + ", should be string, object, or expression.");
		return;
	}

	std::string object_type = current.value;

	std::vector<TypedValue> arguments;
	next_token();
	if (!line_end()) {
		arguments.push_back(parse_expression());
		next_token();
		
		while (!line_end()) {
			expect(comma);
			next_token();
			arguments.push_back(parse_expression());
			next_token();
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

void Parser::parse_connection(std::string name)
{
	std::string output_object = name;

	expect(numeric_literal);
	int output_index = std::stoi(current.value);
	expect(close_brace);

	expect(arrow);
	next_token();
	while (current.type != identifier)
	{
		TokenType operation = current.type;
		next_token();
		TypedValue value = parse_expression();
		std::string name = "inline_object" + std::to_string(inline_object_index++);
		std::vector<TypedValue> argument = { value };

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
		expect(arrow);
		next_token();
	}
	std::string input_object = current.value;

	expect(open_brace);
	expect(numeric_literal);
	int input_index = std::stoi(current.value);
	expect(close_brace);
	expect(newline);
	Program::connect_objects(*program, output_object, output_index, input_object, input_index);
}

TypedValue Parser::parse_expression()
{
	TypedValue value = parse_product();
	while (peek(plus) || peek(minus)) {
		next_token();
		bool subtract = current.type == minus;
		next_token();
		TypedValue operand = parse_product();

		if (subtract) value -= operand;
		else value += operand;
	}
	return value;
}

TypedValue Parser::parse_product()
{
	TypedValue value = parse_factor();
	while (peek(asterisk) || peek(slash)) {
		next_token();
		bool divide = current.type == slash;
		next_token();
		TypedValue operand = parse_factor();

		if (divide) value /= operand;
		else value *= operand;
	}
	return value;
}

TypedValue Parser::parse_factor()
{
	TypedValue value = 0;
	if (current.type == identifier) {
		if (program->symbol_exists(current.value)) value = program->get_symbol_value(current.value);
		else error("Symbol not found: " + current.value);
	}

	else if (current.type == numeric_literal) value = std::stof(current.value);
	else if (current.type == string_literal) value = current.value;

	else if (current.type == open_paren) {
		next_token();
		value = parse_expression();
		expect(close_paren);
	}
	else if (current.type == open_bracket) value = parse_sequence();
	else if (current.type == minus) {
		next_token();
		value = -parse_product().get_value<float>();
	}
	else error("Couldn't get value of expression factor of type " + debug_names[current.type]);

	if (peek(open_bracket)) {
		expect(open_bracket);
		if (!value.is_type<Sequence>()) error("Attempted to subscript non-sequence");
		next_token();

		float index = parse_expression().get_value<float>();
		value = value.get_value<Sequence>().data[(int)index];
		expect(close_bracket);
	}
	return value;
}

Sequence Parser::parse_sequence()
{
	Sequence s;
	next_token();
	s.data.push_back(parse_expression().get_value<float>());

	while (peek(comma)) {
		expect(comma);
		next_token();
		s.data.push_back(parse_expression().get_value<float>());
	}
	
	expect(close_bracket);
	return s;
}

bool Parser::line_end()
{
	return (current.type == newline || current.type == eof);
}

void Parser::error(std::string error)
{
	log(std::to_string(line) + ": " + error);
	program->reset();
	throw ParseException();
}

Token Parser::next_token()
{
	current = get_next_token();
	return current;
}

void Parser::expect(TokenType expected)
{
	next_token();
	verify(expected);
}

void Parser::verify(TokenType expected)
{
	if (current.type != expected) error("Got " + debug_names[current.type] + ", expected " + debug_names[expected]);
}

}

