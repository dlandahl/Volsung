
#include "Parser.hh"
#include "Objects.hh"

namespace Volsung {

const char* VolsungException::what() const noexcept
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
	if (current() == '=') {
		position++;
		if (current() == '>') return { parallel, "" };
		position--;
		return { invalid, "" };
	}
	if (current() == 'x') {
		position++;
		if (current() == '>') return { cross_connection, "" };
		position--;
	}
	if (current() == '.') {
		position++;
		if (current() == '.') return { elipsis, "" };
		position--;
		return { dot, "" };
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
	if (current() == '|') return { vertical_bar, "" };

	if (is_digit()) {
		std::string value;
		value += current();
		position++;
		while (is_digit()) {
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

    error("Unrecognised Token: " + std::to_string(current()));
	return { invalid, "" };
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
	return (current() >= 'a' && current() <= 'z')
		|| current() == '_'
		|| (current() >= 'A' && current() <= 'Z');
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




bool Parser::parse_program(Graph& graph)
{
	program = &graph;
	program->reset();
	program->add_symbol("sf", sample_rate);
	program->add_symbol("tau", TAU);
	try {

	while (true) {
		while (peek(newline)) next_token();
		if (peek(eof)) break;
		if (peek(identifier)) {
			next_token();
			if (peek(colon)) parse_declaration();
			else if (peek(vertical_bar) || peek(arrow) || peek(newline) ||
			         peek(many_to_one) || peek(one_to_many) || peek(parallel)
					 || peek(open_bracket) || peek(cross_connection)) parse_connection();
			else {
				next_token();
				error("Expected colon or connection operator, got " + debug_names[current.type]);
			}
		}
		else if (peek(object)) {
			next_token();
			parse_connection();
		}
		else if (peek(ampersand)) {
			parse_directive();
		}
		else {
			next_token();
			error("Expected declaration or connection, got " + debug_names[current.type]);
		}
	}

	} catch (const VolsungException& exception) {
		log(std::string(exception.what()));
		program->reset();
		return false;
	}
	return true;
}

void Parser::parse_declaration()
{
	std::string name = current.value;
	expect(colon);
	if (peek(numeric_literal)
		|| peek(minus)
		|| peek(string_literal)
		|| peek(open_brace)
		|| peek(open_paren)
		|| peek(identifier)) {
		next_token();
		TypedValue value = parse_expression();
		program->add_symbol(name, value);
		return;

	}
	else if (peek(open_bracket)) {
		expect(open_bracket);
		next_token();
		int count = (int) parse_expression().get_value<float>();
		expect(close_bracket);

		expect(object);
		std::string object_type = current.value;

		if (!peek(newline)) {
			next_token();
			Sequence parameters;
			TypedValue param = parse_expression();
			if (param.is_type<Sequence>()) parameters = param.get_value<Sequence>();
			else for (uint n = 0; n < count; n++) {
				parameters.data.push_back(param.get_value<float>());
			}
			if (parameters.size() != count) error("Sequence initialising group is not the same size as the group");

			for (uint n = 0; n < count; n++)
				make_object(object_type, "__grp_" + name + std::to_string(n), { TypedValue(parameters.data[n]) });
		}
		else {
			for (uint n = 0; n < count; n++)
				make_object(object_type, "__grp_" + name + std::to_string(n), { });
		}

		program->group_sizes[name] = count;
		return;
		
	}
	else if (!peek(object)) {
		next_token();
		error("RHS of declaration is " + debug_names[current.type] + ", should be string, object, or expression.");
		return;
	}
	
	expect(object);
	std::string object_type = current.value;

	std::vector<TypedValue> arguments;
	next_token();
	if (!line_end()) {
		arguments.push_back(parse_expression());
	}

	make_object(object_type, name, arguments);
}

void Parser::make_object(std::string object_type, std::string object_name, std::vector<TypedValue> arguments)
{
	if (object_type == "osc") program->create_object<OscillatorObject>(object_name, arguments);
	else if (object_type == "add")   program->create_object<AddObject>(object_name, arguments);
	else if (object_type == "sqr")program->create_object<SquareObject>(object_name, arguments);
	else if (object_type == "ddl") program->create_object<DelayObject>(object_name, arguments);
	else if (object_type == "mult")  program->create_object<MultObject>(object_name, arguments);
	else if (object_type == "sub")   program->create_object<SubtractionObject>(object_name, arguments);
	else if (object_type == "div")   program->create_object<DivisionObject>(object_name, arguments);
	else if (object_type == "pwr")   program->create_object<PowerObject>(object_name, arguments);
	else if (object_type == "noise") program->create_object<NoiseObject>(object_name, arguments);
	else if (object_type == "sat")   program->create_object<DriveObject>(object_name, arguments);
	else if (object_type == "clock") program->create_object<ClockObject>(object_name, arguments);
	else if (object_type == "timer") program->create_object<TimerObject>(object_name, arguments);
	else if (object_type == "mod")   program->create_object<ModuloObject>(object_name, arguments);
	else if (object_type == "abs")   program->create_object<AbsoluteValueObject>(object_name, arguments);
	else if (object_type == "comp")  program->create_object<ComparatorObject>(object_name, arguments);
	else if (object_type == "pole")program->create_object<FilterObject>(object_name, arguments);
	else if (object_type == "file")  program->create_object<FileoutObject>(object_name, arguments);
	else if (object_type == "step")  program->create_object<StepSequence>(object_name, arguments);
	else if (object_type == "int")  program->create_object<RoundObject>(object_name, arguments);
	else if (object_type == "seq")program->create_object<SequenceObject>(object_name, arguments);
	else if (object_type == "snh")  program->create_object<SampleAndHoldObject>(object_name, arguments);
	else if (object_type == "eg")  program->create_object<EnvelopeObject>(object_name, arguments);
	else if (object_type == "const")  program->create_object<ConstObject>(object_name, arguments);
	else if (object_type == "saw")  program->create_object<SawObject>(object_name, arguments);
	else if (object_type == "tri")  program->create_object<TriangleObject>(object_name, arguments);
	else if (object_type == "lpf")  program->create_object<LowpassObject>(object_name, arguments);
	else if (object_type == "hpf")  program->create_object<HighpassObject>(object_name, arguments);
	else if (object_type == "bpf")  program->create_object<BandpassObject>(object_name, arguments);
	else if (object_type == "env")  program->create_object<EnvelopeFollowerObject>(object_name, arguments);
	else error("No such object type: " + object_type);
}

void Parser::parse_connection()
{
	bool got_newline = false;
	std::string output_object = get_object_to_connect(), input_object;;
	int output_index = 0, input_index = 0;
	ConnectionType connection_type;
	if (peek(vertical_bar)) {
		expect(vertical_bar);
		expect(numeric_literal);
		output_index = std::stoi(current.value);
	} else (output_index = 0);
	
	while (peek(newline)) next_token();
	do {
		next_token();
		if (current.type == arrow) connection_type = ConnectionType::one_to_one;
		else if (current.type == many_to_one) connection_type = ConnectionType::many_to_one;
		else if (current.type == one_to_many) connection_type = ConnectionType::one_to_many;
		else if (current.type == parallel) connection_type = ConnectionType::many_to_many;
		else if (current.type == cross_connection) connection_type = ConnectionType::cross;
		else error("Expected connection operator");
		
		if (peek(numeric_literal)) {
			expect(numeric_literal);
			input_index = std::stoi(current.value);
			expect(vertical_bar);
		} else (input_index = 0);

		next_token();
		input_object = get_object_to_connect();
		program->connect_objects(output_object, output_index, input_object, input_index, connection_type);
		output_object = input_object;

		if (peek(vertical_bar)) {
			expect(vertical_bar);
			expect(numeric_literal);
			output_index = std::stoi(current.value);
		} else (output_index = 0);
		got_newline = false;
		while (peek(newline)) { next_token(); got_newline = true; }

	} while (peek(arrow) || peek(many_to_one) || peek(one_to_many) || peek(parallel));
	if (!got_newline) {
		next_token();
		Volsung::assert(line_end(), "Expected newline or connection operator, got " + debug_names[current.type]);
	}
}

std::string Parser::get_object_to_connect()
{
	std::string output;

	if (current.type == object ||
	    current.type == plus ||
		current.type == minus ||
		current.type == asterisk ||
		current.type == slash ||
		current.type == caret) {

		Token operation = current;
		std::vector<TypedValue> argument = {0};
		if (!(peek(arrow) || peek(newline) || peek(one_to_many) || peek(many_to_one) || peek(parallel))) {
			next_token();
			argument = { parse_expression() };
		}

		output = "__inline_object" + std::to_string(inline_object_index++);

		if (operation.type != object) {
			switch (operation.type) {
				case (plus):     program->create_object<AddObject>(output, argument); break;
				case (minus):    program->create_object<SubtractionObject>(output, argument); break;
				case (asterisk): program->create_object<MultObject>(output, argument); break;
				case (slash):    program->create_object<DivisionObject>(output, argument); break;
				case (caret):    program->create_object<PowerObject>(output, argument); break;
				default: error("Invalid token for inline operation: " + debug_names[operation.type] + ". Expected arithmetic operator");
			}
		} else {
			make_object(operation.value, output, argument);
		}
	}
	else if (current.type == identifier) {
		output = current.value;
		if (peek(open_bracket)) {
			expect(open_bracket);
			next_token();
			int index = parse_number();
			expect(close_bracket);
			output = "__grp_" + output + std::to_string(index);
		}
	}
	else {
		error("Expected inline object declaration or identifier, got " + debug_names[current.type]);
	}

	return output;
}

void Parser::parse_directive()
{
	expect(ampersand);
	expect(identifier);
	std::string directive = current.value;
	std::vector<TypedValue> arguments;
	next_token();
	arguments.push_back(parse_expression());
	while (peek(comma)) {
		expect(comma);
		next_token();
		arguments.push_back(parse_expression());
	}
	program->invoke_directive(directive, arguments);
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

	else if (current.type == numeric_literal) value = parse_number();
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

	if (peek(elipsis)) {
		float const lower = value.get_value<float>();
		expect(elipsis);
		next_token();
		float const upper = parse_expression().get_value<float>();
		float step = 1;
		if (peek(vertical_bar)) {
			next_token();
			next_token();
			step = parse_expression().get_value<float>();
		}
		Sequence s;
		for (float n = lower; n <= upper; n += step) s.data.push_back(n);
		value = s;
	}
	return value;
}

float Parser::parse_number()
{
	std::string number;
	float multiplier = 1;
	verify(numeric_literal);
	number += current.value;
	if (peek(dot)) {
		next_token();
		number += '.';
		expect(numeric_literal);
		number += current.value;
	}
	
	if (peek(identifier)) {
		next_token();
		if (current.value == "s") multiplier = sample_rate;
		if (current.value == "ms") multiplier = sample_rate / 1000;
	}

	return std::stof(number) * multiplier;
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
	Volsung::error(std::to_string(line) + ": " + error);
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
	if (current.type != expected) {
		std::cout << current.value << std::endl;
		error("Got " + debug_names[current.type] + ", expected " + debug_names[expected]);
	}
}

}

