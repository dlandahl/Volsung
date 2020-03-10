
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include "VolsungHeader.hh"

using namespace Volsung;

const std::string Ansi_Red   = "\033[31m";
const std::string Ansi_Green = "\033[32m";
const std::string Ansi_Blue  = "\033[34m";
const std::string Ansi_Reset = "\033[0m";

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
            std::cout << "[" << Ansi_Red << "Failed test" << Ansi_Reset << "] ";
            std::cout << "Parsing " << (std::string) file.path().stem();
            std::cout << "...\nMessage:\n\t" << error_message;
            delete program;
        }

        else {
            std::cout << "[" << Ansi_Green << "Passed test" << Ansi_Reset << "] ";
            std::cout << "Parsing " << (std::string) file.path().stem();
            programs.push_back(program);
            names.push_back((std::string) file.path().stem());
        }

        std::cout << std::endl;
        error_message.clear();
    }

    namespace chrono = std::chrono;
    std::cout << "\nGenerating " << Volsung::sample_rate << " samples\n";
    
    for (size_t p = 0; p < programs.size(); p++) {
        const auto start_time = chrono::high_resolution_clock::now();

        for (size_t s = 0; s < Volsung::sample_rate / Volsung::AudioBuffer::blocksize; s++) {
            programs[p]->run();
        }

        const auto time_taken_usecs = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_time);

        std::cout << "[" << Ansi_Blue << "Timer" << Ansi_Reset << "] ";
        std::cout << names[p];

        for (size_t n = 0; n < 30 - names[p].size(); n++)
            std::cout << ".";

        std::cout << time_taken_usecs.count() / 1000000.f << "s";
        std::cout << std::endl;
        delete programs[p];
    }
    std::cout << std::endl;
}
