
#include <fstream>
#include <iostream>
#include <sstream>

#include "runner.hh"
#include "VolsungHeader.hh"

#if defined(__linux__)
#include "linux.hh"
#endif

std::string get_option(const std::string& command, const std::string& option)
{
    if (command.find(option) == std::string::npos) return "";
    const size_t index = command.find(option) + option.size() + 1;
    if (index >= command.size()) return "";

    const size_t end = command.find(' ', index);
    return command.substr(index, end - index);
}

int main(const int num_arguments, const char* arguments[])
{
     const auto command = [&] () -> std::string {
        std::string command;
        for (int n = 0; n < num_arguments; n++) {
            command += std::string(arguments[n]) + " ";
        }
        return command;
    }();

    const auto filename = [&] () -> std::string {
        std::string name = get_option(command, "-f");
        if (name.empty()) return std::string(arguments[1]);
        return name;
    }();

    if (filename.empty()) {
        std::cout << "Please provide a file name" << std::endl;
        std::exit(1);
    }

    const std::ifstream file(filename);
    if (!file) {
        std::cout << "File could not be read. Maybe it is empty? Or it does not exist?" << std::endl;
        std::exit(1);
    }

    const auto time_seconds = [&] () -> float {
        try {
            std::string time = get_option(command, "-t");
            if (time.empty()) return 5.f;
            return std::stof(time);
        } catch (const std::invalid_argument& e) {
            std::cout << "Please supply a number for the time" << std::endl;
            std::exit(1);
        }
    }();

    Volsung::Program program;
    Volsung::Parser parser;

    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        parser.source_code = buffer.str();
    }

    program.configure_io(0, 1);
    program.reset();
    parser.parse_program(program);
    
    AudioPlayer player;
    player.initialize();

    float data[AudioPlayer::blocksize];
    for (size_t n = 0; n < Volsung::sample_rate / AudioPlayer::blocksize * time_seconds; n++) {
        for (size_t sample = 0; sample < AudioPlayer::blocksize; sample++) {
            data[sample] = program.run();
        }
        std::cout << "Playing chunk " << n << std::endl;
        player.play(data);
    }

    player.clean_up();
    program.finish();
}

