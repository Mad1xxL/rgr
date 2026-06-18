#include <iostream>
#include <string>

void print_help()   {
    std::cout
        << "Cryptum\n"
        << "Использование:\n"
        << "  cryptum [параметры]\n\n"
        << "Параметры:\n"
        << "  --help                  Показать справку\n"
        << "  -a <алгоритм>           rc4 | chacha20\n"
        << "  -m <режим>              encrypt | decrypt\n"
        << "  -k <файл ключа>         Путь к файлу ключа\n"
        << "  -i <входной файл>       Путь к входному файлу\n"
        << "  -o <выходной файл>      Путь к выходному файлу\n";
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

    if (algorithm.empty())  {
        std::cerr << "Ошибка: не указан алгоритм\n";
        return 1;
    }

    if (mode.empty())   {
        std::cerr << "Ошибка: не указан режим работы\n";
        return 1;
    }

    if (algorithm != "rc4" && algorithm != "chacha20")  {
        std::cerr << "Ошибка: алгоритм не поддерживается\n";
        return 1;
    }

    if (mode != "encrypt" && mode != "decrypt") {
        std::cerr << "Ошибка: режим не поддерживается\n";
        return 1;
    }

    if (key_file.empty())   {
        std::cerr << "Ошибка: не указан файл ключа\n";
        return 1;
    }

    if (input_file.empty()) {
        std::cerr << "Ошибка: не указан входной файл\n";
        return 1;
    }

    if (output_file.empty())    {
        std::cerr << "Ошибка: не указан выходной файл\n";
        return 1;
    }

    std::cout << "Алгоритм: " << algorithm << '\n';
    std::cout << "Режим: " << mode << '\n';
    std::cout << "Файл ключа: " << key_file << '\n';
    std::cout << "Входной файл: " << input_file << '\n';
    std::cout << "Выходной файл: " << output_file << '\n';

    return 0;
}