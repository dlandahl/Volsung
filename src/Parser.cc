
#include "Parser.hh"

namespace Volsung {

const char* VolsungException::what() const noexcept
{
    return "Volsung has encountered an error during parsing of the provided program and will exit the parsing stage.";
}


Token Lexer::get_next_token()
{
    position++;
    if (current() == EOF || current() == -1) return { TokenType::eof, "" };

    while (current() == ' ' || current() == '\t') position++;
    if (current() == ';') while (current() != '\n') position++;
    
    switch(current()) {
        case '\n':
            line++;
            return { TokenType::newline, "" };

        case '-':
            position++;
            if (current() == '>') return { TokenType::arrow, "" };
            position--;
            return { TokenType::minus, "" };

        case '>':
            position++;
            if (current() == '>') return { TokenType::many_to_one, "" };
            position--;
            return { TokenType::greater_than, "" };

        case '<':
            position++;
            if (current() == '>') return { TokenType::one_to_many, "" };
            position--;
            return { TokenType::less_than, "" };

        case '=':
            position++;
            if (current() == '>') return { TokenType::parallel, "" };
            position--;
            return { TokenType::invalid, "" };

        case 'x':
            position++;
            if (current() == '>') return { TokenType::cross_connection, "" };
            position--;
            break;

        case '.':
            position++;
            if (current() == '.') return { TokenType::elipsis, "" };
            position--;
            return { TokenType::dot, "" };

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
            if (position >= source_code.size()) break;
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
    if (position >= source_code.size()) return EOF;
    return source_code.at(position);
}

bool Lexer::is_digit() const
{
    return current() >= '0' && current() <= '9';
}

bool Lexer::is_char() const
{
    return (current() >= 'a' && current() <= 'z')
        || (current() >= 'A' && current() <= 'Z')
        ||  current() == '_';
}

bool Lexer::peek(const TokenType expected)
{
    const size_t old_position = position;
    const size_t old_lines = line;
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




using ObjectMap = std::map<std::string, void(Program::*)(const std::string&, const ArgumentList&)>;
#define OBJECT(x) &Program::create_object<x>
static const ObjectMap object_creators =
{
    { "Sine_Oscillator",     OBJECT(OscillatorObject) },
    { "Saw_Oscillator",      OBJECT(SawObject) },
    { "Square_Oscillator",   OBJECT(SquareObject) },
    { "Triangle_Oscillator", OBJECT(TriangleObject) },
    { "Noise",               OBJECT(NoiseObject) },
    { "Constant",            OBJECT(ConstObject) },
    { "Add",                 OBJECT(AddObject) },
    { "Multiply",            OBJECT(MultObject) },
    { "Subtract",            OBJECT(SubtractionObject) },
    { "Divide",              OBJECT(DivisionObject) },
    { "Power",               OBJECT(PowerObject) },
    { "Delay_Line",          OBJECT(DelayObject) },
    { "Clock",               OBJECT(ClockObject) },
    { "Envelope_Follower",   OBJECT(EnvelopeFollowerObject) },
    { "Envelope_Generator",  OBJECT(EnvelopeObject) },
    { "Timer",               OBJECT(TimerObject) },
    { "Tanh",                OBJECT(DriveObject) },
    { "Modulo",              OBJECT(ModuloObject) },
    { "Abs",                 OBJECT(AbsoluteValueObject) },
    { "Floor",               OBJECT(RoundObject) },
    { "Comparator",          OBJECT(ComparatorObject) },
    { "Bi_to_Unipolar",      OBJECT(BiToUnipolarObject) },
    { "Write_File",          OBJECT(FileoutObject) },
    { "Read_File",           OBJECT(FileinObject) },
    { "Step_Sequence",       OBJECT(StepSequence) },
    { "Index_Sequence",      OBJECT(SequenceObject) },
    { "Sample_And_Hold",     OBJECT(SampleAndHoldObject) },
    { "Smooth",              OBJECT(FilterObject) },
    { "Lowpass_Filter",      OBJECT(LowpassObject) },
    { "Highpass_Filter",     OBJECT(HighpassObject) },
    { "Bandpass_Filter",     OBJECT(BandpassObject) },
    { "Allpass_Filter",      OBJECT(AllpassObject) },
    { "Convolver",           OBJECT(ConvolveObject) },
    { "Z_Plane",             OBJECT(ZPlaneObject) },
    { "Pole",                OBJECT(PoleObject) },
    { "Zero",                OBJECT(ZeroObject) }
};
#undef OBJECT


bool Parser::parse_program(Graph& graph)
{
    program = &graph;
    program->add_symbol("sample_rate", sample_rate);
    program->add_symbol("fs", sample_rate);
    program->add_symbol("tau", TAU);
    program->add_symbol("pi", TAU / 2.f);
    program->add_symbol("true", 1);
    program->add_symbol("false", 0);
    program->add_symbol("i", Number(0, 1));
    program->add_symbol("e", 2.718281828459045f);
    program->add_symbol("blocksize", AudioBuffer::blocksize);

    try {

    while (true) {
        while (peek(TokenType::newline)) next_token();

        if (peek(TokenType::eof) || (program->parent && peek(TokenType::close_brace))) break;

        if (peek(TokenType::identifier)) {
            next_token();
            if (peek(TokenType::colon)) parse_declaration();
            else if (peek_connection()) parse_connection();
            else if (peek(TokenType::open_paren)) {
                const std::string id = current_token.value;
                parse_procedure_call(id);
                if (!(peek(TokenType::newline) || peek(TokenType::eof))) error ("Expected newline after procedure call");
                next_token();
            }
            else if (peek(TokenType::less_than)) parse_subgraph_declaration();
            else {
                next_token();
                error("Expected colon or connection operator, got " + debug_names.at(current_token.type));
            }
        }
        
        else if (peek(TokenType::object) || peek(TokenType::open_bracket)) {
            next_token();
            parse_connection();
        }
        
        else if (peek(TokenType::ampersand)) parse_directive();
        else {
            next_token();
            error("Expected declaration or connection, got " + debug_names.at(current_token.type));
        }
    }

    } catch (const VolsungException&) {
        program->reset();
        return false;
    }
    return true;
}

void Parser::parse_declaration()
{
    const std::string name = current_token.value;
    expect(TokenType::colon);

    if (peek_expression()) {
        next_token();
        const TypedValue value = parse_expression();
        program->add_symbol(name, value);
    }

    else if (peek(TokenType::object) || peek(TokenType::open_bracket)) {
        next_token();
        parse_connection(parse_object_declaration(name));
    }

    else {
        next_token();
        error("Expected object, group, or expression, got " + debug_names.at(current_token.type));
    }
}

std::string Parser::parse_object_declaration(std::string name)
{
    if (name.empty()) name = "Unnamed Object " + std::to_string(inline_object_index++);
    bool is_group = false;
    size_t count = 1;

    if (current_token_is(TokenType::open_bracket)) {
        is_group = true;
        next_token();
        count = (int) parse_expression().get_value<Number>();
        expect(TokenType::close_bracket);
        next_token();
    }

    verify(TokenType::object);
    const std::string object_type = current_token.value;

    if (is_group) {
        const size_t old_position = position;

        bool n_existed = false;
        TypedValue old_n;
        if (program->symbol_exists("n")) {
            old_n = program->get_symbol_value("n");
            n_existed = true;
        }

        for (size_t n = 0; n < count; n++) {
            position = old_position;

            program->remove_symbol("n");
            program->add_symbol("n", n+1);

            ArgumentList arguments = { };
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
        ArgumentList arguments = { };
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

void Parser::make_object(const std::string& object_type, const std::string& object_name, const ArgumentList& arguments)
{
    if (object_creators.count(object_type)) {
        (program->*(object_creators.at(object_type)))(object_name, arguments);
        return;
    }

    SubgraphRepresentation subgraph = program->find_subgraph_recursively(object_type);

    auto io = subgraph.second;
    ArgumentList parameters = arguments;
    parameters.insert(parameters.begin(), TypedValue { (Number) io[0] });
    parameters.insert(parameters.begin() + 1, TypedValue { (Number) io[1] });

    program->create_object<SubgraphObject>(object_name, parameters);

    program->get_audio_object_raw_pointer<SubgraphObject>(object_name)->graph = std::make_unique<Program>();
    Program* const other_program = program->get_audio_object_raw_pointer<SubgraphObject>(object_name)->graph.get();

    Parser subgraph_parser;
    subgraph_parser.source_code = subgraph.first;
    other_program->parent = program;
    other_program->configure_io((uint) io[0], (uint) io[1]);
    other_program->reset();

    for (size_t n = 2; n < parameters.size(); n++)
        other_program->add_symbol("_" + std::to_string(n-1), parameters[n]);

    if (!subgraph_parser.parse_program(*other_program)) error("Subgraph failed to parse");
}

void Parser::parse_connection()
{
    std::string output_object = get_object_to_connect();
    parse_connection(output_object);
}

void Parser::parse_connection(std::string output_object)
{
    int output_index = 0, input_index = 0;
    std::string input_object;

    if (peek(TokenType::vertical_bar)) {
        expect(TokenType::vertical_bar);
        expect(TokenType::numeric_literal);
        output_index = std::stoi(current_token.value);
    }
    else (output_index = 0);

    while (peek(TokenType::newline)) {
        next_token();
    }

    ConnectionType connection_type;
    bool got_newline = false;
    int path_length = 0;

    while (peek(TokenType::arrow) || peek(TokenType::many_to_one) || peek(TokenType::one_to_many) || peek(TokenType::parallel) || peek(TokenType::cross_connection)) {
        next_token();

        switch (current_token.type) {
            case TokenType::arrow:            connection_type = ConnectionType::one_to_one; break;
            case TokenType::many_to_one:      connection_type = ConnectionType::many_to_one; break;
            case TokenType::one_to_many:      connection_type = ConnectionType::one_to_many; break;
            case TokenType::parallel:         connection_type = ConnectionType::many_to_many; break;
            case TokenType::cross_connection: connection_type = ConnectionType::biclique; break;
            default: error("Expected connection operator, got " + debug_names.at(current_token.type));
        }

        if (peek(TokenType::numeric_literal)) {
            expect(TokenType::numeric_literal);
            input_index = std::stoi(current_token.value);
            expect(TokenType::vertical_bar);
        }
        else input_index = 0;

        next_token();
        input_object = get_object_to_connect();
        program->connect_objects(output_object, output_index, input_object, input_index, connection_type);
        output_object = input_object;

        if (peek(TokenType::vertical_bar)) {
            expect(TokenType::vertical_bar);
            expect(TokenType::numeric_literal);
            output_index = std::stoi(current_token.value);
        }
        else output_index = 0;

        got_newline = false;
        while (peek(TokenType::newline)) { next_token(); got_newline = true; }
        path_length++;
    };

    if (!got_newline && path_length > 1) {
        next_token();
        Volsung::assert(line_end(), "Expected newline or connection operator, got " + debug_names.at(current_token.type));
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

        const Token operation = current_token;
        next_token();
        const ArgumentList argument = { parse_expression() };

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
        output = current_token.value;
        
        if (peek(TokenType::colon)) {
            next_token();
            next_token();
            parse_object_declaration(output);
        }
        Volsung::assert(program->object_exists(output), "Undefined object: " + output);
        
        if (peek(TokenType::open_bracket)) {
            expect(TokenType::open_bracket);
            next_token();
            const int index = (int) parse_number();
            expect(TokenType::close_bracket);
            output = "__grp_" + output + std::to_string(index);
        }
    }

    else if (current_token_is(TokenType::object) || current_token_is(TokenType::open_bracket)) {
            output = parse_object_declaration();
    }

    else {
        error("Expected inline object declaration or identifier, got " + debug_names.at(current_token.type));
    }

    return output;
}

void Parser::parse_directive()
{
    expect(TokenType::ampersand);
    expect(TokenType::identifier);
    const std::string directive = current_token.value;
    ArgumentList arguments;

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
    const std::string name = current_token.value;

    expect(TokenType::less_than);
    next_token();
    const float inputs = parse_expression().get_value<Number>();
    expect(TokenType::comma);
    next_token();
    const float outputs = parse_expression().get_value<Number>();
    expect(TokenType::greater_than);
    expect(TokenType::colon);
    expect(TokenType::open_brace);
    expect(TokenType::newline);
    
    const size_t start_position = position;
    int num_braces_encountered = 0;
    
    while (true) {
        position++;
        if (current() == '}') {
            if (!num_braces_encountered) break;
            num_braces_encountered--;
        }
        if (current() == '\n') line++;
        if (current() == EOF || current() == -1) error("Program ended with incomplete subgraph definition");
        if (current() == '{') num_braces_encountered++;
    }

    position--;
    program->subgraphs.insert({ name, { source_code.substr(start_position, position - start_position), { inputs, outputs } } });
    expect(TokenType::close_brace);
}

TypedValue Parser::parse_expression()
{
    TypedValue value = parse_sequence_generator();
    while (peek(TokenType::plus) || peek(TokenType::minus)) {
        next_token();
        const bool subtract = current_token_is(TokenType::minus);
        next_token();
        const TypedValue operand = parse_sequence_generator();

        if (subtract) value -= operand;
        else value += operand;
    }
    return value;
}

TypedValue Parser::parse_sequence_generator()
{
    TypedValue value = parse_product();
    if (peek(TokenType::elipsis)) {
        expect(TokenType::elipsis);
        next_token();

        const float lower = value.get_value<Number>();
        const float upper = parse_product().get_value<Number>();

        if (peek(TokenType::elipsis)) {
            next_token();
            next_token();
            const float target = parse_product().get_value<Number>();
            const float step_size = (target - lower) / (upper - 1);
            Sequence s;
            for (size_t n = 0; n < upper; n++)
                s.add_element(lower + n * step_size);
            value = s;
        }
        
        else {
            float step = 1;
            if (peek(TokenType::vertical_bar)) {
                next_token();
                next_token();
                step = parse_product().get_value<Number>();
            }

            Sequence s;
            if (lower > upper) for (float n = lower; n >= upper; n -= step) s.add_element(n);
            else               for (float n = lower; n <= upper; n += step) s.add_element(n);
            value = s;
        }
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
    switch (current_token.type) {
        case TokenType::identifier: {
            const std::string id = current_token.value;
            if (peek(TokenType::open_paren)) value = parse_procedure_call(id);
            else if (program->symbol_exists(id)) value = program->get_symbol_value(id);
            else error("Symbol not found: " + id);
            break;
        }

        case TokenType::numeric_literal: value = parse_number(); break;
        case TokenType::string_literal:  value = current_token.value; break;

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

        default: error("Couldn't get value of expression factor of token type " + debug_names.at(current_token.type));
    }

    if (peek(TokenType::open_bracket)) {
        expect(TokenType::open_bracket);
        if (!value.is_type<Sequence>()) error("Attempted to subscript non-sequence");
        next_token();

        const TypedValue index = parse_expression();
        expect(TokenType::close_bracket);

        if (index.is_type<Number>()) {
            value = value.get_value<Sequence>()[(int) index.get_value<Number>()];
        }

        else if (index.is_type<Sequence>()) {
            Sequence s;
            const Sequence& index_sequence = index.get_value<Sequence>();
            const Sequence& value_sequence = value.get_value<Sequence>();

            for (size_t n = 0; n < index_sequence.size(); n++)
                s.add_element(value_sequence[(size_t) index_sequence[(size_t) n]]);

            value = s;
        }
        
        else error("Index into sequence must be a number or a sequence");
    }


    return value;
}

Number Parser::parse_number()
{
    verify(TokenType::numeric_literal);
    std::string number_string = current_token.value;

    if (peek(TokenType::dot)) {
        next_token();
        number_string += '.';
        expect(TokenType::numeric_literal);
        number_string += current_token.value;
    }

    float number = std::stof(number_string);
    float multiplier = 1;
    bool imaginary = false;
    if (peek(TokenType::identifier)) {
        next_token();
        if (current_token.value == "s")       multiplier = sample_rate;
        else if (current_token.value == "ms") multiplier = sample_rate / 1000;
        else if (current_token.value == "deg") multiplier = TAU / 360.0;
        else if (current_token.value == "i") imaginary = true;
        else if (current_token.value == "dB"
              || current_token.value == "db") number = std::log10(number * 20);
        else error("Invalid literal operator or stray identifier");
    }

    if (imaginary) return Number(0, number);
    return number * multiplier;
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

TypedValue Parser::parse_procedure_call(const std::string& name)
{
    ArgumentList arguments;
    expect(TokenType::open_paren);
    if (peek_expression()) {
        next_token();
        arguments.push_back(parse_expression());
    }

    while (peek(TokenType::comma)) {
        expect(TokenType::comma);
        next_token();
        arguments.push_back(parse_expression());
    }
    expect(TokenType::close_paren);

    if (!Program::procedures.count(name)) error("Procedure does not exist: '" + name + "'");
    auto& procedure = Program::procedures.at(name);
    
    if (procedure.max_arguments < arguments.size())
        Volsung::error("Too many arguments in procedure call to '" + name +"'. Expected " + std::to_string(procedure.max_arguments) +", got " + std::to_string(arguments.size()));

    if (procedure.min_arguments > arguments.size())
        Volsung::error("Too few arguments in procedure call to '" + name +"'. Expected " + std::to_string(procedure.min_arguments) +", got " + std::to_string(arguments.size()));

    return procedure(arguments, program);
}

bool Parser::line_end() const
{
    return current_token_is(TokenType::newline) || current_token_is(TokenType::eof);
}

bool Parser::current_token_is(TokenType type) const
{
    return current_token.type == type;
}

void Parser::error(const std::string& error) const
{
    Volsung::error("Line " + std::to_string(line) + ": " + error);
}

Token Parser::next_token()
{
    current_token = get_next_token();
    return current_token;
}

void Parser::expect(TokenType expected)
{
    next_token();
    verify(expected);
}

void Parser::verify(TokenType expected) const
{
    if (!current_token_is(expected)) {
        error("Got " + debug_names.at(current_token.type) + ", expected " + debug_names.at(expected));
    }
}

}
