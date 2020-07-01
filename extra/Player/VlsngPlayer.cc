
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <charconv>

#include "Volsung.hh"

#if defined(__linux__)
#include "AudioPlayer_Linux.hh"
#elif defined(__APPLE__)
#include "AudioPlayer_Macos.hh"
#elif defined(_WIN32)
#include "AudioPlayer_Windows.hh"
#endif

int main(const int num_args, const char* args[])
{
    const std::vector<std::string> arguments(args, args + num_args);

    std::string filename;
    float time_seconds = 5.f;
    size_t num_channels = 1;

    std::map<std::string, float> parameters;
    bool offline = false;
    bool dont_run = false;
    bool profile = false;

    Volsung::set_library_path("../extra/standard_library/");

    for (int n = 1; n < num_args; n++) {
        const std::string arg = arguments[n];

        if (arg[0] != '-') {
            if (filename.empty()) filename = arg;
            else std::cout << "Unexpected command-line argument: " << arg << ". Filename is already set. Ignoring." << std::endl;
            continue;
        }

        try {
            auto next_arg = [&] () -> std::string { return arguments.at(size_t(n)+1); };
            if (arg == "-f" || arg == "--file") {
                if (!filename.empty())
                    std::cout << "Unexpected argument '" << filename << "' since -f flag is being used to set filename. Ignoring." << std::endl;
                filename = next_arg();
            }
            else if (arg == "-t" || arg == "--time") time_seconds = std::stof(next_arg());
            else if (arg == "-c" || arg == "--channels") num_channels = std::stoi(next_arg());
            else if (arg == "-o" || arg == "--offline") offline = true;
            else if (arg == "-n" || arg == "--compile") dont_run = true;
            else if (               arg == "--profile") profile = true;
            else if (arg == "-p" || arg == "--parameter") {
                const std::string& key_value = next_arg();
                const size_t pos = key_value.find("=");
                const std::string key = key_value.substr(0, pos);
                const std::string value = key_value.substr(pos + 1, key_value.size() - pos);
                parameters[key] = std::stof(value);
            }
            else std::cout << "Flag not recognised: " << arg << ". Ignoring." << std::endl;
            n++;
        } 
        catch (std::out_of_range&) {
            std::cout << "Flag expected an argument, none found. Exiting." << std::endl;
            std::exit(1);
        }
        catch (std::invalid_argument&) {
            std::cout << "Could not read numeric parameter. Did you need to type a number? Exiting." << std::endl;
            std::exit(1);
        }
    }

    if (filename.empty()) {
        std::cout << "Please provide a file name. Exiting." << std::endl;
        std::exit(1);
    }

    const std::ifstream file(filename);
    if (!file) {
        std::cout << "File could not be read. Maybe it is empty? Or it does not exist?" << std::endl;
        std::exit(1);
    }

    Volsung::set_debug_callback([] (const std::string& message) {
        std::cout << message << std::endl;
    });

    Volsung::Program program;
    Volsung::Parser parser;

    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        parser.source_code = buffer.str();
    }

    program.configure_io(0, num_channels);
    program.reset();
    for (auto& [key, value] : parameters) {
        program.add_symbol(key, value);
    }

    if (!parser.parse_program(program)) std::exit(0);
    if (dont_run) std::exit(0);

    AudioPlayer player;
    player.initialize((Volsung::uint) num_channels);

    float* data = new float[Volsung::AudioBuffer::blocksize];

    auto const play_one_block = [&] () {
        Volsung::MultichannelBuffer buffer = program.run({ Volsung::AudioBuffer::zero });
        std::copy(buffer[0].begin(), buffer[0].end(), data);
        if (offline) return;
        player.play(data);
    };

    if (time_seconds == -1.f) {
        while (true) play_one_block();
    }

    else {
        const auto blocks_to_generate = Volsung::get_sample_rate() / AudioPlayer::blocksize * time_seconds;
        for (size_t n = 0; n < blocks_to_generate; n++)
            play_one_block();
    }

    player.clean_up();
    program.finish();
    delete[] data;
}
