
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include "VolsungHeader.hh"

using namespace Volsung;

const std::string ANSI_RED   = "\033[31m";
const std::string ANSI_GREEN = "\033[32m";
const std::string ANSI_BLUE  = "\033[34m";
const std::string ANSI_RESET = "\033[0m";

int main()
{
    std::string error_message;
    debug_callback = [&error_message] (std::string message) { error_message += message + "\n"; };

    std::vector<Program*> programs;
    std::vector<std::string> names;
    
    const std::filesystem::path path = "../test/test_programs";
    std::cout << "\nAttempting to parse some programs\n";
    
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
            std::cout << "...\nMessage:\n\t" << error_message;
            delete program;
        }

        else {
            std::cout << "[" << ANSI_GREEN << "Passed test" << ANSI_RESET << "] ";
            std::cout << "Parsing " << (std::string) file.path().stem();
            programs.push_back(program);
            names.push_back((std::string) file.path().stem());
        }

        std::cout << std::endl;
        error_message.clear();
    }

    using hrc = std::chrono::high_resolution_clock;
    std::cout << "\nGenerating " << Volsung::sample_rate << " samples\n";
    
    for (std::size_t p = 0; p < programs.size(); p++) {
        const auto start_time = hrc::now();

        for (std::size_t s = 0; s < Volsung::sample_rate; s++) {
            programs[p]->run();
        }
        
        const auto time_taken_usecs = std::chrono::duration_cast<std::chrono::microseconds>(hrc::now() - start_time).count();

        std::cout << "[" << ANSI_BLUE << "Timer" << ANSI_RESET << "] ";
        std::cout << names[p];

        for (std::size_t n = 0; n < 30 - names[p].size(); n++)
            std::cout << ".";

        std::cout << time_taken_usecs / 1000000.f << "s";
        std::cout << std::endl;
        delete programs[p];
    }
    std::cout << std::endl;
}

