#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>

#include <dlfcn.h>
#include "include/crypto_api.h"


bool read_binary_file(const std::string& path, std::vector<uint8_t>& data)  {
    std::ifstream file(path, std::ios::binary);

    if (!file)  {
        return false;
    }

    file.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    data.resize(size);

    if (size > 0)   {
        file.read(reinterpret_cast<char*>(data.data()), size);
    }
    return true;
}

bool write_binary_file(const std::string& path, const std::vector<uint8_t>& data)   {
    std::ofstream file(path, std::ios::binary);

    if (!file)  {
        return false;
    }

    if (!data.empty())  {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    return true;
}

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

    std::vector<uint8_t> key_data;
    std::vector<uint8_t> input_data;

    if (!read_binary_file(key_file, key_data))  {
        std::cerr << "Ошибка: не удалось открыть файл ключа\n";
        return 1;
    }

    if (!read_binary_file(input_file, input_data))  {
        std::cerr << "Ошибка: не удалось открыть входной файл\n";
        return 1;
    }

    std::cout << "Размер ключа: " << key_data.size() << " байт\n";
    std::cout << "Размер входного файла: " << input_data.size() << " байт\n";

    std::string library_path;
    if (algorithm == "rc4") {
        library_path = "algorithms/rc4/librc4.dylib";
    }

    else if (algorithm == "chacha20")   {
        library_path = "algorithms/chacha20/libchacha20.dylib";
    }

    void* library = dlopen(library_path.c_str(), RTLD_LAZY);
    
    if (library == nullptr) {
        std::cerr << "Ошибка: не удалось загрузить библиотеку\n";
        return 1;
    }
    
    std::cout << "Библиотека успешно загружена\n";

    using GetAlgorithmInfoFunc = const AlgorithmInfo* (*)();
    GetAlgorithmInfoFunc get_algorithm_info = reinterpret_cast<GetAlgorithmInfoFunc>(dlsym(library, "get_algorithm_info"));
    
    using EncryptFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);
    EncryptFunc encrypt_function = reinterpret_cast<EncryptFunc>(dlsym(library, "encrypt"));

    if (get_algorithm_info == nullptr)  {
        std::cerr << "Ошибка: не удалось получить функцию get_algorithm_info\n";
        dlclose(library);
        return 1;
    }

    if (encrypt_function == nullptr)    {
        std::cerr << "Ошибка: не удалось получить функцию encrypt\n";
        dlclose(library);
        return 1;
    }

    const AlgorithmInfo* info = get_algorithm_info();

    std::cout << "Название алгоритма из библиотеки: " << info->algorithm_name << '\n';

    std::cout << "Размер ключа из библиотеки: " << info->key_size << " байт\n";

    std::cout << "Функция encrypt успешно загружена\n";

    std::vector<uint8_t> output_data(input_data.size());

    ConstBuffer key_buffer  {
        key_data.data(),
        key_data.size()
    };

    ConstBuffer input_buffer    {
        input_data.data(),
        input_data.size()
    };

    MutBuffer output_buffer {
        output_data.data(),
        output_data.size()
    };

    int result = encrypt_function(key_buffer, input_buffer, &output_buffer);

    if (result < 0) {
        std::cerr << "Ошибка: шифрование завершилось с кодом " << result << '\n';
        dlclose(library);
        return 1;
    }

    dlclose(library);

    if (!write_binary_file(output_file, output_data))   {
        std::cerr << "Ошибка: не удалось записать выходной файл\n";
        return 1;
    }

    std::cout << "Выходной файл успешно записан\n";
    
    return 0;
}