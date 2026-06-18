#include <iostream>
#include <string>

void print_help()   {
    std::cout
        << "Cryptum\n"
        << "Usage:\n"
        << "  cryptum [options]\n\n"
        << "Options:\n"
        << "  --help                Show help message\n"
        << "  -a <algorithm>        rc4 | chacha20\n"
        << "  -m <mode>             encrypt | decrypt\n"
        << "  -k <key_file>         Path to key file\n"
        << "  -i <input_file>       Path to input file\n"
        << "  -o <output_file>      Path to output file\n";
}

int main(int argc, char* argv[])    {
    if (argc == 1)  {
        print_help();
        return 0;
    }

    std::string first_argument = argv[1];

    if (first_argument == "--help") {
        print_help();
        return 0;
    }

    std::string algorithm;
    std::string mode;
    std::string key_file;
    std::string input_file;
    std::string output_file;

    for (int i = 1; i < argc; ++i)  {
        std::string argument = argv[i];

        if (argument == "-a" && i + 1 < argc)   {
            algorithm = argv[++i];
        }
        else if (argument == "-m" && i + 1 < argc)  {
            mode = argv[++i];
        }
        else if (argument == "-k" && i + 1 < argc)  {
            key_file = argv[++i];
        }
        else if (argument == "-i" && i + 1 < argc)  {
            input_file = argv[++i];
        }
        else if (argument == "-o" && i + 1 < argc)  {
            output_file = argv[++i];
        }
    }

    std::cout << "Algorithm: " << algorithm << '\n';
    std::cout << "Mode: " << mode << '\n';
    std::cout << "Key file: " << key_file << '\n';
    std::cout << "Input file: " << input_file << '\n';
    std::cout << "Output file: " << output_file << '\n';

    return 0;
}