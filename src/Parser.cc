
#include "Parser.hh"
#include "Objects.hh"

namespace Volsung {

const char* VolsungException::what() const noexcept
{
    return "Volsung has encountered an error during parsing of the provided program and will exit the parsing stage.";
}

Token Lexer::get_next_token()
{
    if (position >= source_code.size() - 1) return { TokenType::eof, "" };
    position++;

    while (current() == ' ' || current() == '\t') position++;
    if (current() == ';') while (current() != '\n') position++;

    if (current() == '\n') {
        line++;
        return { TokenType::newline, "" };
    }

    if (current() == '-') {
        position++;
        if (current() == '>') return { TokenType::arrow, "" };
        position--;
        return { TokenType::minus, "" };
    }
    if (current() == '>') {
        position++;
        if (current() == '>') return { TokenType::many_to_one, "" };
        position--;
        return { TokenType::greater_than, "" };
    }
    if (current() == '<') {
        position++;
        if (current() == '>') return { TokenType::one_to_many, "" };
        position--;
        return { TokenType::less_than, "" };
    }
    if (current() == '=') {
        position++;
        if (current() == '>') return { TokenType::parallel, "" };
        position--;
        return { TokenType::invalid, "" };
    }
    if (current() == 'x') {
        position++;
        if (current() == '>') return { TokenType::cross_connection, "" };
        position--;
    }
    if (current() == '.') {
        position++;
        if (current() == '.') return { TokenType::elipsis, "" };
        position--;
        return { TokenType::dot, "" };
    }

    switch(current()) {
        case '{': return { TokenType::open_brace, "" };
        case '}': return { TokenType::close_brace, "" };
        case '(': return { TokenType::open_paren, "" };
        case ')': return { TokenType::close_paren, "" };
        case '[': return { TokenType::open_bracket, "" };
        case ']': return { TokenType::close_bracket, "" };
        case ':': return { TokenType::colon, "" };
        case ',': return { TokenType::comma, "" };
        case '&': return { TokenType::ampersand, "" };
        case '*': return { TokenType::asterisk, "" };
        case '+': return { TokenType::plus, "" };
        case '/': return { TokenType::slash, "" };
        case '^': return { TokenType::caret, "" };
        case '|': return { TokenType::vertical_bar, "" };
    }

    if (is_digit()) {
        std::string value;
        value += current();
        position++;
        while (is_digit()) {
            value += current();
            position++;
        }
        position--;
        return { TokenType::numeric_literal, value };
    }

    if (is_char()) {
        std::string id;
        while (is_char() || is_digit()) {
            id += current();
            position++;
        }
        if (current() == '~') return { TokenType::object, id };
        position--;
        return { TokenType::identifier, id };
    }

    if (current() == '"') {
        std::string string;
        position++;
        while (current() != '"') {
            string += current();
            position++;
        }
        return { TokenType::string_literal, string };
    }

    error("Unrecognised Token: " + std::to_string(current()));
    return { TokenType::invalid, "" };
}

char Lexer::current() const
{
    return source_code.at(position);
}

bool Lexer::is_digit() const
{
    return current() >= '0' && current() <= '9';
}

bool Lexer::is_char() const
{
    return (current() >= 'a' && current() <= 'z')
        || current() == '_'
        || (current() >= 'A' && current() <= 'Z');
}

bool Lexer::peek(const TokenType expected)
{
    const std::size_t old_position = position;
    const std::size_t old_lines = line;
    bool ret;
    if (get_next_token().type == expected) ret = true;
    else ret = false;
    position = old_position;
    line = old_lines;
    return ret;
}

bool Lexer::peek_expression()
{
    return peek(TokenType::numeric_literal) || peek(TokenType::minus)
        || peek(TokenType::string_literal)  || peek(TokenType::open_brace)
        || peek(TokenType::open_paren)      || peek(TokenType::identifier);
}

bool Lexer::peek_connection()
{
    return peek(TokenType::vertical_bar)
        || peek(TokenType::arrow)
        || peek(TokenType::newline)
        || peek(TokenType::many_to_one)
        || peek(TokenType::one_to_many)
        || peek(TokenType::parallel)
        || peek(TokenType::cross_connection)
        || peek(TokenType::open_bracket);
}

Lexer::~Lexer() {}




bool Parser::parse_program(Graph& graph)
{
    program = &graph;
    program->add_symbol("sample_rate", sample_rate);
    program->add_symbol("fs", sample_rate);
    program->add_symbol("tau", TAU);
    try {

    while (true) {
        while (peek(TokenType::newline)) next_token();

        if (peek(TokenType::eof) || (program->parent && peek(TokenType::close_brace))) break;

        if (peek(TokenType::identifier)) {
            next_token();
            if (peek(TokenType::colon)) parse_declaration();
            else if (peek_connection()) {
                  parse_connection();
            }

            else if (peek(TokenType::open_paren)) {
                parse_subgraph_declaration();
            }

            else {
                next_token();
                error("Expected colon or connection operator, got " + debug_names.at(current.type));
            }
        }
        else if (peek(TokenType::object) || peek(TokenType::open_bracket)) {
            next_token();
            parse_connection();
        }
        else if (peek(TokenType::ampersand)) parse_directive();
        else {
            next_token();
            error("Expected declaration or connection, got " + debug_names.at(current.type));
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
    const std::string name = current.value;
    expect(TokenType::colon);

    if (peek_expression()) {
        next_token();
        const TypedValue value = parse_expression();
        program->add_symbol(name, value);
    }

    else {
        next_token();
        parse_object_declaration(name);
    }
}

std::string Parser::parse_object_declaration(std::string name)
{
    if (name.empty()) name = "Unnamed Object " + std::to_string(inline_object_index++);
    bool is_group = false;
    std::size_t count = 1;

    if (current_token_is(TokenType::open_bracket)) {
        is_group = true;
        next_token();
        count = (int) parse_expression().get_value<Number>();
        expect(TokenType::close_bracket);
        next_token();
    }

    verify(TokenType::object);
    const std::string object_type = current.value;

    if (is_group) {
        const std::size_t old_position = position;

        bool n_existed = false;
        TypedValue old_n;
        if (program->symbol_exists("n")) {
            old_n = program->get_symbol_value("n");
            n_existed = true;
        }

        for (std::size_t n = 0; n < count; n++) {
            position = old_position;

            program->remove_symbol("n");
            program->add_symbol("n", n+1);

            std::vector<TypedValue> arguments = { };
            if (peek_expression()) {
                next_token();
                arguments.push_back(parse_expression());
            }

            while (peek(TokenType::comma)) {
                expect(TokenType::comma);
                next_token();
                arguments.push_back(parse_expression());
            }

            make_object(object_type, "__grp_" + name + std::to_string(n), arguments);
            program->remove_symbol("n");
        }
        program->group_sizes.insert({ name, count });

        if (n_existed) program->add_symbol("n", old_n);
    }

    else {
        if (program->group_sizes.count(name)) error("Object " + name + " already exists as group");
        std::vector<TypedValue> arguments = { };
        if (peek_expression()) {
            next_token();
            arguments.push_back(parse_expression());
        }

        while (peek(TokenType::comma)) {
            expect(TokenType::comma);
            next_token();
            arguments.push_back(parse_expression());
        }

        make_object(object_type, name, arguments);
    }
    return name;
}

void Parser::make_object(const std::string& object_type, const std::string& object_name, const std::vector<TypedValue>& arguments)
{
    if (object_type == "Sine_Oscillator")            program->create_object<OscillatorObject>(object_name, arguments);
    else if (object_type == "Saw_Oscillator")        program->create_object<SawObject>(object_name, arguments);
    else if (object_type == "Square_Oscillator")     program->create_object<SquareObject>(object_name, arguments);
    else if (object_type == "Triangle_Oscillator")   program->create_object<TriangleObject>(object_name, arguments);
    else if (object_type == "Noise")                 program->create_object<NoiseObject>(object_name, arguments);
    else if (object_type == "Constant")              program->create_object<ConstObject>(object_name, arguments);
    else if (object_type == "Add")                   program->create_object<AddObject>(object_name, arguments);
    else if (object_type == "Multiply")              program->create_object<MultObject>(object_name, arguments);
    else if (object_type == "Subtract")              program->create_object<SubtractionObject>(object_name, arguments);
    else if (object_type == "Divide")                program->create_object<DivisionObject>(object_name, arguments);
    else if (object_type == "Power")                 program->create_object<PowerObject>(object_name, arguments);
    else if (object_type == "Delay_Line")            program->create_object<DelayObject>(object_name, arguments);
    else if (object_type == "Clock_Generator")       program->create_object<ClockObject>(object_name, arguments);
    else if (object_type == "Envelope_Generator")    program->create_object<EnvelopeFollowerObject>(object_name, arguments);
    else if (object_type == "Envelope_Follower")     program->create_object<EnvelopeObject>(object_name, arguments);
    else if (object_type == "Timer")                 program->create_object<TimerObject>(object_name, arguments);
    else if (object_type == "Tanh")                  program->create_object<DriveObject>(object_name, arguments);
    else if (object_type == "Modulo")                program->create_object<ModuloObject>(object_name, arguments);
    else if (object_type == "Absolute_Value")        program->create_object<AbsoluteValueObject>(object_name, arguments);
    else if (object_type == "Floor")                 program->create_object<RoundObject>(object_name, arguments);
    else if (object_type == "Comparator")            program->create_object<ComparatorObject>(object_name, arguments);
    else if (object_type == "File")                  program->create_object<FileoutObject>(object_name, arguments);
    else if (object_type == "Step_Sequence")         program->create_object<StepSequence>(object_name, arguments);
    else if (object_type == "Index_Sequence")        program->create_object<SequenceObject>(object_name, arguments);
    else if (object_type == "Sample_And_Hold")       program->create_object<SampleAndHoldObject>(object_name, arguments);
    else if (object_type == "Pole")                  program->create_object<FilterObject>(object_name, arguments);
    else if (object_type == "Lowpass_Filter")        program->create_object<LowpassObject>(object_name, arguments);
    else if (object_type == "Highpass_Filter")       program->create_object<HighpassObject>(object_name, arguments);
    else if (object_type == "Bandpass_Filter")       program->create_object<BandpassObject>(object_name, arguments);
    else if (object_type == "Allpass_Filter")        program->create_object<AllpassObject>(object_name, arguments);
    else if (program->subgraphs.count(object_type)) {
        auto io = program->subgraphs[object_type].second;
        std::vector<TypedValue> parameters = arguments;
        parameters.insert(parameters.begin(), TypedValue { (Number) io[0] });
        parameters.insert(parameters.begin() + 1, TypedValue { (Number) io[1] });

        program->create_object<SubgraphObject>(object_name, arguments);

        program->get_audio_object_raw_pointer<SubgraphObject>(object_name)->graph = std::make_unique<Program>();
        Program* const other_program = program->get_audio_object_raw_pointer<SubgraphObject>(object_name)->graph.get();

        Parser subgraph_parser;
        subgraph_parser.source_code = program->subgraphs[object_type].first;
        other_program->parent = program;
        other_program->configure_io(io[0], io[1]);
        other_program->reset();

        for (std::size_t n = 2; n < parameters.size(); n++)
            other_program->add_symbol("_" + std::to_string(n-1), parameters[n]);

        if (!subgraph_parser.parse_program(*other_program)) error("Subgraph failed to parse");
    }
    else error("No such object type: " + object_type);
}

void Parser::parse_connection()
{
    int output_index= 0, input_index = 0;
    std::string output_object = get_object_to_connect(), input_object;

    if (peek(TokenType::vertical_bar)) {
        expect(TokenType::vertical_bar);
        expect(TokenType::numeric_literal);
        output_index = std::stoi(current.value);
    } else (output_index = 0);

    while (peek(TokenType::newline)) {
        next_token();
    }

    ConnectionType connection_type;
    bool got_newline = false;

    do {
        next_token();

        if (current_token_is(TokenType::arrow))                 connection_type = ConnectionType::one_to_one;
        else if (current_token_is(TokenType::many_to_one))      connection_type = ConnectionType::many_to_one;
        else if (current_token_is(TokenType::one_to_many))      connection_type = ConnectionType::one_to_many;
        else if (current_token_is(TokenType::parallel))         connection_type = ConnectionType::many_to_many;
        else if (current_token_is(TokenType::cross_connection)) connection_type = ConnectionType::biclique;
        else error("Expected connection operator, got " + debug_names.at(current.type));

        if (peek(TokenType::numeric_literal)) {
            expect(TokenType::numeric_literal);
            input_index = std::stoi(current.value);
            expect(TokenType::vertical_bar);
        } else input_index = 0;

        next_token();
        input_object = get_object_to_connect();
        program->connect_objects(output_object, output_index, input_object, input_index, connection_type);
        output_object = input_object;

        if (peek(TokenType::vertical_bar)) {
            expect(TokenType::vertical_bar);
            expect(TokenType::numeric_literal);
            output_index = std::stoi(current.value);
        } else output_index = 0;

        got_newline = false;
        while (peek(TokenType::newline)) { next_token(); got_newline = true; }

    } while (peek(TokenType::arrow) || peek(TokenType::many_to_one) || peek(TokenType::one_to_many) || peek(TokenType::parallel));

    if (!got_newline) {
        next_token();
        Volsung::assert(line_end(), "Expected newline or connection operator, got " + debug_names.at(current.type));
    }
}

std::string Parser::get_object_to_connect()
{
    std::string output;
    
    if (current_token_is(TokenType::plus)
     || current_token_is(TokenType::minus)
     || current_token_is(TokenType::asterisk)
     || current_token_is(TokenType::slash)
     || current_token_is(TokenType::caret)) {

        const Token operation = current;
        next_token();
        const std::vector<TypedValue> argument = { parse_expression() };

        output = "Unnamed Object " + std::to_string(inline_object_index++);

        if (operation.type != TokenType::object) {
            switch (operation.type) {
                case (TokenType::plus):     program->create_object<AddObject>(output, argument); break;
                case (TokenType::minus):    program->create_object<SubtractionObject>(output, argument); break;
                case (TokenType::asterisk): program->create_object<MultObject>(output, argument); break;
                case (TokenType::slash):    program->create_object<DivisionObject>(output, argument); break;
                case (TokenType::caret):    program->create_object<PowerObject>(output, argument); break;
                default: error("Invalid token for inline operation: " + debug_names.at(operation.type) + ". Expected arithmetic operator");
            }
        } else {
            make_object(operation.value, output, argument);
        }
    }

    else if (current_token_is(TokenType::identifier)) {
        output = current.value;
        if (peek(TokenType::colon)) {
            next_token();
            next_token();
            parse_object_declaration(output);
        }
        
        if (peek(TokenType::open_bracket)) {
            expect(TokenType::open_bracket);
            next_token();
            const int index = parse_number();
            expect(TokenType::close_bracket);
            output = "__grp_" + output + std::to_string(index);
        }
    }

    else if (current_token_is(TokenType::object) || current_token_is(TokenType::open_bracket)) {
            output = parse_object_declaration();
    }

    else {
        error("Expected inline object declaration or identifier, got " + debug_names.at(current.type));
    }

    return output;
}

void Parser::parse_directive()
{
    expect(TokenType::ampersand);
    expect(TokenType::identifier);
    const std::string directive = current.value;
    std::vector<TypedValue> arguments;

    if (!peek(TokenType::newline)) {
        next_token();
        arguments.push_back(parse_expression());

        while (peek(TokenType::comma)) {
            expect(TokenType::comma);
            next_token();
            arguments.push_back(parse_expression());
        }
    }
    program->invoke_directive(directive, arguments);
}

void Parser::parse_subgraph_declaration()
{
    const std::string name = current.value;

    expect(TokenType::open_paren);
    next_token();
    const float inputs = parse_expression().get_value<Number>();
    expect(TokenType::comma);
    next_token();
    const float outputs = parse_expression().get_value<Number>();
    expect(TokenType::close_paren);
    expect(TokenType::colon);
    expect(TokenType::open_brace);

    const std::size_t start_position = position;
    int num_braces_encountered = 0;

    while (true) {
        position++;
        if (source_code[position] == '}') {
            if (!num_braces_encountered) break;
            num_braces_encountered--;
        }
        if (source_code[position] == '\n') line++;
        if (source_code[position] == '{') num_braces_encountered++;
    }

    position--;
    program->subgraphs.insert({ name, { source_code.substr(start_position, position - start_position), { inputs, outputs } } });
    expect(TokenType::close_brace);
}

TypedValue Parser::parse_expression()
{
    TypedValue value = parse_product();
    while (peek(TokenType::plus) || peek(TokenType::minus)) {
        next_token();
        const bool subtract = current_token_is(TokenType::minus);
        next_token();
        const TypedValue operand = parse_product();

        if (subtract) value -= operand;
        else value += operand;
    }
    return value;
}

TypedValue Parser::parse_product()
{
    TypedValue value = parse_power();
    while (peek(TokenType::asterisk) || peek(TokenType::slash)) {
        next_token();
        const bool divide = current_token_is(TokenType::slash);
        next_token();
        const TypedValue operand = parse_power();

        if (divide) value /= operand;
        else value *= operand;
    }
    return value;
}

TypedValue Parser::parse_power()
{
    TypedValue value = parse_factor();
    if (peek(TokenType::caret)) {
        expect(TokenType::caret);
        next_token();
        const TypedValue operand = parse_power();

        value ^= operand;
    }
    return value;
}

TypedValue Parser::parse_factor()
{
    TypedValue value = 0;
    switch (current.type) {
        case TokenType::identifier:
            if (program->symbol_exists(current.value)) value = program->get_symbol_value(current.value);
            else error("Symbol not found: " + current.value);
            break;

        case TokenType::numeric_literal: value = parse_number(); break;
        case TokenType::string_literal:  value = current.value; break;

        case TokenType::open_paren:
            next_token();
            value = parse_expression();
            expect(TokenType::close_paren);
            break;

        case TokenType::open_brace: value = parse_sequence(); break;
        case TokenType::minus:
            next_token();
            value = -parse_product();
            break;

        default: error("Couldn't get value of expression factor of type " + debug_names.at(current.type));
    }

    if (peek(TokenType::open_bracket)) {
        expect(TokenType::open_bracket);
        if (!value.is_type<Sequence>()) error("Attempted to subscript non-sequence");
        next_token();

        const TypedValue index = parse_expression();
        expect(TokenType::close_bracket);

        if (index.is_type<Number>()) {
            value = value.get_value<Sequence>()[(int) index.get_value<Number>()];
        } else if (index.is_type<Sequence>()) {
            Sequence s;
            const Sequence& index_sequence = index.get_value<Sequence>();
            const Sequence& value_sequence = value.get_value<Sequence>();

            for (std::size_t n = 0; n < index_sequence.size(); n++)
                s.add_element(value_sequence[index_sequence[n]]);

            value = s;
        }
    }

    if (peek(TokenType::elipsis)) {
        expect(TokenType::elipsis);
        next_token();

        float const lower = value.get_value<Number>();
        float const upper = parse_expression().get_value<Number>();
        float step = 1;

        if (peek(TokenType::vertical_bar)) {
            next_token();
            next_token();
            step = parse_expression().get_value<Number>();
        }

        Sequence s;
        if (lower > upper) for (float n = lower; n >= upper; n -= step) s.add_element(n);
        else for (float n = lower; n <= upper; n += step) s.add_element(n);
        value = s;
    }
    return value;
}

Number Parser::parse_number()
{
    float multiplier = 1;
    verify(TokenType::numeric_literal);
    std::string number = current.value;

    if (peek(TokenType::dot)) {
        next_token();
        number += '.';
        expect(TokenType::numeric_literal);
        number += current.value;
    }

    if (peek(TokenType::identifier)) {
        next_token();
        if (current.value == "s") multiplier = sample_rate;
        else if (current.value == "ms") multiplier = sample_rate / 1000;
        else error("Invalid literal operator or stray identifier");
    }

    return std::stof(number) * multiplier;
}

Sequence Parser::parse_sequence()
{
    Sequence s;
    next_token();
    s.add_element(parse_expression().get_value<Number>());

    while (peek(TokenType::comma)) {
        expect(TokenType::comma);
        next_token();
        s.add_element(parse_expression().get_value<Number>());
    }

    expect(TokenType::close_brace);
    return s;
}

bool Parser::line_end() const
{
    return current_token_is(TokenType::newline) || current_token_is(TokenType::eof);
}

bool Parser::current_token_is(TokenType type) const
{
    return current.type == type;
}

void Parser::error(const std::string& error) const
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

void Parser::verify(TokenType expected) const
{
    if (!current_token_is(expected)) {
        error("Got " + debug_names.at(current.type) + ", expected " + debug_names.at(expected));
    }
}

}
