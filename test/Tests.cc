
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include "Volsung.hh"

using namespace Volsung;
namespace chrono = std::chrono;

const std::string Ansi_Red     = "\033[31m";
const std::string Ansi_Green   = "\033[32m";
const std::string Ansi_Yellow  = "\033[33m";
const std::string Ansi_Reset   = "\033[0m";

int main()
{
    std::string error_message;
    set_debug_callback([&error_message] (std::string message) { error_message += message + "\n"; });

    std::vector<Program*> programs;
    std::vector<std::string> names;
    
    const std::filesystem::path path = "../test/test_programs";
    std::filesystem::current_path(path);

    std::cout << "\nAttempting to parse some programs\n";

    Program::add_directive("length", [] (std::vector<TypedValue>, Program*) {
        return;
    });

    const size_t num_dots = 30;

    for (const auto& file : std::filesystem::directory_iterator(".")) {
        Parser parser;
        Program* program = new Program;
        program->configure_io(0, 2);

        program->reset();

        {
            std::stringstream buffer;
            buffer << std::ifstream(file.path()).rdbuf();
            parser.source_code = buffer.str();
        }

        const auto name = (std::string) file.path().stem();
        std::cout << "Parsing " << name;
        std::cout << std::flush;
        for (size_t n = 0; n < num_dots - name.size(); n++)
            std::cout << ".";

        const auto start_time = chrono::high_resolution_clock::now();

        if (!parser.parse_program(*program)) {
            std::cout << "[" << Ansi_Red << "Fail" << Ansi_Reset << "] ";
            std::cout << "...\nMessage:\n\t" << error_message;
            delete program;
        }

        else {
            const auto time_taken_usecs = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_time);

            std::cout << "[" << Ansi_Green << "Pass" << Ansi_Reset << ": ";
            std::cout << time_taken_usecs.count() / 1000000.f << "s]";
            programs.push_back(program);
            names.push_back(name);
        }

        std::cout << std::endl;
        error_message.clear();
    }

    std::cout << "\nGenerating " << Volsung::get_sample_rate() << " samples\n";

    for (size_t p = 0; p < programs.size(); p++) {
        const auto start_time = chrono::high_resolution_clock::now();

        for (size_t s = 0; s < Volsung::get_sample_rate() / Volsung::AudioBuffer::blocksize; s++) {
            programs[p]->run();
        }

        const auto time_taken_usecs = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_time);

        std::cout << "Running " << names[p];

        for (size_t n = 0; n < num_dots - names[p].size(); n++)
            std::cout << ".";

        const auto time = time_taken_usecs.count() / 1000000.f;
        std::cout << "["  << Ansi_Green << "Pass"  << Ansi_Reset << ": " << time << "s]";
        std::cout << std::endl;
        delete programs[p];
    }
    std::cout << std::endl;
}
