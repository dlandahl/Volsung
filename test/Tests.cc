
#include <iostream>
#include <fstream>
#include <array>
#include <filesystem>

#include "VolsungHeader.hh"

using namespace Volsung;

const std::string ANSI_RED   = "\033[31m";
const std::string ANSI_GREEN = "\033[32m";
const std::string ANSI_RESET = "\033[0m";

int main()
{
    std::string error_message;
    debug_callback = [&error_message] (std::string message) { error_message += message + "\n"; };

    std::vector<Program*> programs;
    
    const std::filesystem::path path = "../test/test_programs";
    for (const auto& file : std::filesystem::directory_iterator(path)) {
        Parser parser;
        Program* program = new Program;
        program->configure_io(0, 2);
        program->reset();

        {
            std::stringstream buffer;
            buffer << std::ifstream(file.path()).rdbuf();
            parser.source_code = buffer.str();
        }
        
        if (!parser.parse_program(*program)) {
            std::cout << "[" << ANSI_RED << "Failed test" << ANSI_RESET << "] ";
            std::cout << "Parsing " << (std::string) file.path().stem();
            std::cout << ". Message:\n\t" << error_message;
            delete program;
        }
        else {
            std::cout << "[" << ANSI_GREEN << "Passed test" << ANSI_RESET << "] ";
            std::cout << "Parsing " << (std::string) file.path().stem();
            programs.push_back(program);
        }

        std::cout << std::endl;
        error_message.clear();
    }

    for (auto program : programs) {
        for (std::size_t n = 0; n < 100; n++) {
            program->run();
        }
    }
}

