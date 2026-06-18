#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>

#include "include/crypto_api.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


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

std::vector<uint8_t> generate_key(size_t size)  {
    std::vector<uint8_t> key(size);
    std::random_device random_device;

    for (size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<uint8_t>(random_device());
    }

    return key;
}

void print_help()   {
    std::cout
        << "Cryptum\n"
        << "Использование:\n"
        << "  cryptum [параметры]\n\n"
        << "Параметры:\n"
        << "  --help                            Показать справку\n"
        << "  -a, --algorithm <алгоритм>        rc4 | chacha20\n"
        << "  -m, --mode <режим>                encrypt | decrypt | generate-key\n"
        << "  -k, --key <файл ключа>            Путь к файлу ключа\n"
        << "  -i, --input <входной файл>        Путь к входному файлу\n"
        << "  -o, --output <выходной файл>      Путь к выходному файлу\n";
}

#ifdef _WIN32
using LibraryHandle = HMODULE;
#else
using LibraryHandle = void*;
#endif

LibraryHandle open_library(const std::string& path) {
#ifdef _WIN32
    return LoadLibraryA(path.c_str());
#else
    return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

void* load_function(LibraryHandle library, const char* name)    {
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(library, name));
#else
    return dlsym(library, name);
#endif
}

void close_library(LibraryHandle library)   {
#ifdef _WIN32
    FreeLibrary(library);
#else
    dlclose(library);
#endif
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

        if ((argument == "-a" || argument == "--algorithm") && i + 1 < argc)    {
            algorithm = argv[++i];
        }
        else if ((argument == "-m" || argument == "--mode") && i + 1 < argc)    {
            mode = argv[++i];
        }
        else if ((argument == "-k" || argument == "--key") && i + 1 < argc) {
            key_file = argv[++i];
        }
        else if ((argument == "-i" || argument == "--input") && i + 1 < argc)   {
            input_file = argv[++i];
        }
        else if ((argument == "-o" || argument == "--output") && i + 1 < argc)  {
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

    if (mode != "encrypt" && mode != "decrypt" && mode != "generate-key") {
        std::cerr << "Ошибка: режим не поддерживается\n";
        return 1;
    }

    if (mode != "generate-key" && key_file.empty()) {
        std::cerr << "Ошибка: не указан файл ключа\n";
        return 1;
    }

    if (mode != "generate-key" && input_file.empty())   {
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

    std::string library_path;

    if (algorithm == "rc4") {
    #ifdef _WIN32
        library_path = "algorithms/rc4/rc4.dll";
    #elif __APPLE__
        library_path = "algorithms/rc4/librc4.dylib";
    #else
        library_path = "algorithms/rc4/librc4.so";
    #endif
    }
    else if (algorithm == "chacha20")   {
    #ifdef _WIN32
        library_path = "algorithms/chacha20/chacha20.dll";
    #elif __APPLE__
        library_path = "algorithms/chacha20/libchacha20.dylib";
    #else
        library_path = "algorithms/chacha20/libchacha20.so";
    #endif
    }
    
    LibraryHandle library = open_library(library_path);

    if (library == nullptr) {
        std::cerr << "Ошибка: не удалось загрузить библиотеку\n";
        return 1;
    }

    std::cout << "Библиотека успешно загружена\n";

    using GetAlgorithmInfoFunc = const AlgorithmInfo* (*)();
    GetAlgorithmInfoFunc get_algorithm_info = reinterpret_cast<GetAlgorithmInfoFunc>(load_function(library, "get_algorithm_info"));
    
    using EncryptFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);
    EncryptFunc encrypt_function = reinterpret_cast<EncryptFunc>(load_function(library, "encrypt"));

    using DecryptFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);
    DecryptFunc decrypt_function = reinterpret_cast<DecryptFunc>(load_function(library, "decrypt"));

    using GetOutputSizeFunc = size_t (*)(size_t, int);
    GetOutputSizeFunc get_output_size = reinterpret_cast<GetOutputSizeFunc>(load_function(library, "get_output_size"));

    if (get_algorithm_info == nullptr)  {
        std::cerr << "Ошибка: не удалось получить функцию get_algorithm_info\n";
        close_library(library);
        return 1;
    }
    
    if (get_output_size == nullptr) {
        std::cerr << "Ошибка: не удалось получить функцию get_output_size\n";
        close_library(library);
        return 1;
    }

    if (encrypt_function == nullptr)    {
        std::cerr << "Ошибка: не удалось получить функцию encrypt\n";
        close_library(library);
        return 1;
    }

    if (decrypt_function == nullptr)    {
        std::cerr << "Ошибка: не удалось получить функцию decrypt\n";
        close_library(library);
        return 1;
    }

    const AlgorithmInfo* info = get_algorithm_info();

    std::cout << "Название алгоритма из библиотеки: " << info->algorithm_name << '\n';

    std::cout << "Размер ключа из библиотеки: " << info->key_size << " байт\n";

    if (mode == "generate-key") {
        std::vector<uint8_t> generated_key = generate_key(info->key_size);

        if (!write_binary_file(output_file, generated_key)) {
            std::cerr << "Ошибка: не удалось записать файл ключа\n";
            close_library(library);
            return 1;
        }

        std::cout << "Ключ успешно сгенерирован\n";

        close_library(library);
        return 0;
    }

    std::vector<uint8_t> key_data;
    std::vector<uint8_t> input_data;

    if (!read_binary_file(key_file, key_data))   {
        std::cerr << "Ошибка: не удалось открыть файл ключа\n";
        close_library(library);
        return 1;
    }

    if (!read_binary_file(input_file, input_data))  {
        std::cerr << "Ошибка: не удалось открыть входной файл\n";
        close_library(library);
        return 1;
    }

    std::cout << "Размер ключа: " << key_data.size() << " байт\n";
    std::cout << "Размер входного файла: " << input_data.size() << " байт\n";
    std::cout << "Функции encrypt и decrypt успешно загружены\n";

    int operation_type = 0;

    if (mode == "decrypt")  {
        operation_type = 1;
    }
    size_t output_size = get_output_size(input_data.size(), operation_type);
    std::vector<uint8_t> output_data(output_size);

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

    int result = 0;

    if (mode == "encrypt")  {
        result = encrypt_function(key_buffer, input_buffer, &output_buffer);
    }
    else    {
        result = decrypt_function(key_buffer, input_buffer, &output_buffer);
    }

    if (result < 0) {
        std::cerr << "Ошибка: операция завершилась с кодом " << result << '\n';
        close_library(library);
        return 1;
    }

    close_library(library);

    if (!write_binary_file(output_file, output_data))   {
        std::cerr << "Ошибка: не удалось записать выходной файл\n";
        return 1;
    }

    std::cout << "Выходной файл успешно записан\n";

    return 0;
}