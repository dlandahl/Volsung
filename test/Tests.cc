
#include <iostream>
#include <fstream>
#include <array>
#include <filesystem>

#include "VolsungHeader.hh"

using namespace Volsung;

int main()
{
    Parser parser;
    Program program;
    program.configure_io(0, 1);

    std::string error_message;
    debug_callback = [&error_message] (std::string message) { error_message += message + "\n"; };
    
    const std::filesystem::path path = "../test/test_programs";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
    
        program.reset();

        {
            std::stringstream buffer;
            buffer << std::ifstream(entry.path()).rdbuf();
            parser.source_code = buffer.str();
        }
        
        if (!parser.parse_program(program)) {
            std::cout << "Failed test: Parsing " << (std::string) entry.path().stem() << ". Message:\n\t";
            std::cout << error_message;
        }
        else {
            std::cout << "Passed test: Parsing " << (std::string) entry.path().stem();
        }
        std::cout << std::endl;
    }
}

