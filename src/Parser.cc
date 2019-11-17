
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

	while (current() == ' ' || current() == '\t') position++;
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
	if (current() == '>') {
		position++;
		if (current() == '>') return { many_to_one, "" };
		position--;
		return { greater_than, "" };
	}
	if (current() == '<') {
		position++;
		if (current() == '>') return { one_to_many, "" };
		position--;
		return { less_than, "" };
	}
	
	if (current() == '{') return { open_brace, "" };
	if (current() == '}') return { close_brace, "" };
	if (current() == '(') return { open_paren, "" };
	if (current() == ')') return { close_paren, "" };
	if (current() == '[') return { open_bracket, "" };
	if (current() == ']') return { close_bracket, "" };
	if (current() == '<') return { greater_than, "" };
	if (current() == ':') return { colon, "" };
	if (current() == ',') return { comma, "" };
	if (current() == '&') return { ampersand, "" };
	if (current() == '*') return { asterisk, "" };
	if (current() == '+') return { plus, "" };
	if (current() == '/') return { slash, "" };
	if (current() == '^') return { caret, "" };

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
		while (is_char() || is_digit()) {
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
	return current() >= 'a' && current() <= 'z'
		|| current() == '_'
		|| current() >= 'A' && current() <= 'Z';
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
			else if (current.type == open_paren) parse_connection(id);
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
		program->reset();
	}
}

void Parser::parse_declaration(std::string name)
{
	std::string object_name = name;

	next_token();
	if (current.type == numeric_literal
		|| current.type == minus
		|| current.type == string_literal
		|| current.type == open_brace
		|| current.type == open_paren
		|| current.type == identifier) {
		TypedValue value = parse_expression();
		program->add_symbol(name, value);
		return;

	} else if (current.type == open_bracket) {
		next_token();
		int count = (int) parse_expression().get_value<float>();
		expect(close_bracket);

		expect(object);
		std::string object_type = current.value;
		next_token();
		Sequence parameters = parse_expression().get_value<Sequence>();
		if (parameters.size() != count) error("Sequence initialising group is not the same size as the group");

		for (uint n = 0; n < count; n++)
			make_object(object_type, "__grp_" + object_name + std::to_string(n), { TypedValue(parameters.data[n]) });

		program->group_sizes[object_name] = count;
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
	}

	make_object(object_type, object_name, arguments);
}

void Parser::make_object(std::string object_type, std::string object_name, std::vector<TypedValue> arguments)
{
	if (object_type == "osc") program->create_object<OscillatorObject>(object_name, arguments);
	else if (object_type == "add")   program->create_object<AddObject>(object_name, arguments);
	else if (object_type == "square")program->create_object<SquareObject>(object_name, arguments);
	else if (object_type == "delay") program->create_object<DelayObject>(object_name, arguments);
	else if (object_type == "mult")  program->create_object<MultObject>(object_name, arguments);
	else if (object_type == "sub")   program->create_object<SubtractionObject>(object_name, arguments);
	else if (object_type == "div")   program->create_object<DivisionObject>(object_name, arguments);
	else if (object_type == "noise") program->create_object<NoiseObject>(object_name, arguments);
	else if (object_type == "sat")   program->create_object<DriveObject>(object_name, arguments);
	else if (object_type == "clock") program->create_object<ClockObject>(object_name, arguments);
	else if (object_type == "timer") program->create_object<TimerObject>(object_name, arguments);
	else if (object_type == "mod")   program->create_object<ModuloObject>(object_name, arguments);
	else if (object_type == "abs")   program->create_object<AbsoluteValueObject>(object_name, arguments);
	else if (object_type == "comp")  program->create_object<ComparatorObject>(object_name, arguments);
	else if (object_type == "filter")program->create_object<FilterObject>(object_name, arguments);
	else if (object_type == "file")  program->create_object<FileoutObject>(object_name, arguments);
	else if (object_type == "step")  program->create_object<StepSequence>(object_name, arguments);
	else error("No such object type: " + object_type);
}

void Parser::parse_connection(std::string name)
{
	std::string output_object = name;

	expect(numeric_literal);
	int output_index = std::stoi(current.value);
	expect(close_paren);

	next_token();
	while (current.type == newline) next_token();
	if (current.type != many_to_one &&
		current.type != one_to_many &&
		current.type != arrow) error("Expected connection operator");
	TokenType connection_type = current.type;
	
	next_token();
	while (current.type != identifier)
	{
		std::vector<TypedValue> argument = {0};
		Token operation = current;
		if (!(peek(arrow) || peek(newline))) {
			next_token();
			argument = { parse_expression() };
		}
		std::string name = "__inline_object" + std::to_string(inline_object_index++);

		if (operation.type != object) {
			switch (operation.type) {
				case (plus):     program->create_object<AddObject>(name, argument); break;
				case (minus):    program->create_object<SubtractionObject>(name, argument); break;
				case (asterisk): program->create_object<MultObject>(name, argument); break;
				case (slash):    program->create_object<DivisionObject>(name, argument); break;
				default: error("Invalid token for inline operation: " + debug_names[operation.type] + ". Expected arithmetic operator");
			}
		} else {
			make_object(operation.value, name, argument);
		}

		Program::connect_objects(*program, output_object, output_index, name, 0);
		output_index = 0;
		output_object = name;
		while (peek(newline)) next_token();
		expect(arrow);
		next_token();
	}
	std::string input_object = current.value;

	expect(open_paren);
	expect(numeric_literal);
	int input_index = std::stoi(current.value);
	expect(close_paren);
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
	TypedValue value = parse_power();
	while (peek(asterisk) || peek(slash)) {
		next_token();
		bool divide = current.type == slash;
		next_token();
		TypedValue operand = parse_power();

		if (divide) value /= operand;
		else value *= operand;
	}
	return value;
}

TypedValue Parser::parse_power()
{
	TypedValue value = parse_factor();
	if (peek(caret)) {
		expect(caret);
		next_token();
		TypedValue operand = parse_power();

		value ^= operand;
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
	else if (current.type == open_brace) value = parse_sequence();
	else if (current.type == minus) {
		next_token();
		value = -parse_product();
	}
	else error("Couldn't get value of expression factor of type " + debug_names[current.type]);

	if (peek(open_bracket)) {
		expect(open_bracket);
		if (!value.is_type<Sequence>()) error("Attempted to subscript non-sequence");
		next_token();

		TypedValue index = parse_expression();
		expect(close_bracket);

		if (index.is_type<float>()) {
			value = value.get_value<Sequence>().data[(int)index.get_value<float>()];
		} else if (index.is_type<Sequence>()) {
			Sequence s;
			Sequence& index_sequence = index.get_value<Sequence>();
			Sequence& value_sequence = value.get_value<Sequence>();
			for (uint n = 0; n < index_sequence.size(); n++)
				s.data.push_back(value_sequence.data[index_sequence.data[n]]);

			value = s;
		}
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
	
	expect(close_brace);
	return s;
}

bool Parser::line_end()
{
	return (current.type == newline || current.type == eof);
}

void Parser::error(std::string error)
{
	log(std::to_string(line) + ": " + error);
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

