#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "include/crypto_api.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <dlfcn.h>
#endif

bool read_binary_file(const std::string& path, std::vector<uint8_t>& data) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();

    if (file_size < 0) {
        return false;
    }

    file.seekg(0, std::ios::beg);

    size_t size = static_cast<size_t>(file_size);
    data.resize(size);

    if (size > 0) {
        file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));

        if (!file) {
            return false;
        }
    }

    return true;
}

bool write_binary_file(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    if (!data.empty()) {
        file.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
    }

    return static_cast<bool>(file);
}

bool read_stdin(std::vector<uint8_t>& data) {
    char symbol;

    while (std::cin.get(symbol)) {
        data.push_back(static_cast<uint8_t>(symbol));
    }

    return std::cin.eof();
}

bool write_stdout(const std::vector<uint8_t>& data) {
    if (!data.empty()) {
        std::cout.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
    }

    std::cout.flush();
    return static_cast<bool>(std::cout);
}

void secure_clear(std::vector<uint8_t>& data) {
    volatile uint8_t* pointer = data.data();

    for (size_t i = 0; i < data.size(); ++i) {
        pointer[i] = 0;
    }

    data.clear();
}

std::vector<uint8_t> generate_key(size_t size) {
    std::vector<uint8_t> key(size);
    std::random_device random_device;

    for (size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<uint8_t>(random_device());
    }

    return key;
}

void print_help() {
    std::cout
        << "Cryptum\n"
        << "Использование:\n"
        << "  cryptum [параметры]\n\n"
        << "Параметры:\n"
        << "  -h, --help                        Показать справку\n"
        << "  -a, --algorithm <алгоритм>        rc4 | chacha20 | atbash | vernam | vigenere | elgamal\n"
        << "  -m, --mode <режим>                encrypt | decrypt | generate-key\n"
        << "  -k, --key <файл ключа>            Путь к файлу ключа или - для stdin\n"
        << "  -i, --input <входной файл>        Путь к входному файлу или - для stdin\n"
        << "  -o, --output <выходной файл>      Путь к выходному файлу или - для stdout\n"
        << "  -s, --save-key <файл>             Сохранить сгенерированный ключ в файл\n"
        << "  -w, --write-key                   Вывести сгенерированный ключ в stdout\n"
        << "  -g, --generate-key                Сгенерировать ключ\n\n"
        << "Примечание:\n"
        << "  Алгоритм atbash не использует ключ.\n";
}

bool is_supported_algorithm(const std::string& algorithm) {
    return algorithm == "rc4" ||
           algorithm == "chacha20" ||
           algorithm == "atbash" ||
           algorithm == "vernam" ||
           algorithm == "vigenere" ||
           algorithm == "elgamal";
}

std::string make_library_path(const std::string& algorithm) {
    std::string directory = "algorithms/" + algorithm + "/";

#ifdef _WIN32
    return directory + algorithm + ".dll";
#elif __APPLE__
    return directory + "lib" + algorithm + ".dylib";
#else
    return directory + "lib" + algorithm + ".so";
#endif
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

void* load_function(LibraryHandle library, const char* name) {
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(library, name));
#else
    return dlsym(library, name);
#endif
}

void close_library(LibraryHandle library) {
#ifdef _WIN32
    FreeLibrary(library);
#else
    dlclose(library);
#endif
}

void set_binary_mode_for_standard_streams() {
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

int main(int argc, char* argv[]) {
    set_binary_mode_for_standard_streams();

    if (argc == 1) {
        print_help();
        return 0;
    }

    std::string algorithm;
    std::string mode;
    std::string key_file;
    std::string input_file;
    std::string output_file;

    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];

        if (argument == "-h" || argument == "--help") {
            print_help();
            return 0;
        }
        else if (argument == "-a" || argument == "--algorithm") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать алгоритм\n";
                return 1;
            }

            algorithm = argv[++i];
        }
        else if (argument == "-m" || argument == "--mode") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать режим\n";
                return 1;
            }

            mode = argv[++i];
        }
        else if (argument == "-k" || argument == "--key") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать файл ключа\n";
                return 1;
            }

            key_file = argv[++i];
        }
        else if (argument == "-i" || argument == "--input") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать входной файл\n";
                return 1;
            }

            input_file = argv[++i];
        }
        else if (argument == "-o" || argument == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать выходной файл\n";
                return 1;
            }

            output_file = argv[++i];
        }
        else if (argument == "-s" || argument == "--save-key") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << argument << " нужно указать файл для ключа\n";
                return 1;
            }

            output_file = argv[++i];
        }
        else if (argument == "-w" || argument == "--write-key") {
            output_file = "-";
        }
        else if (argument == "-g" || argument == "--generate-key") {
            mode = "generate-key";
        }
        else {
            std::cerr << "Ошибка: неизвестный аргумент: " << argument << '\n';
            return 1;
        }
    }

    if (algorithm.empty()) {
        std::cerr << "Ошибка: не указан алгоритм\n";
        return 1;
    }

    if (!is_supported_algorithm(algorithm)) {
        std::cerr << "Ошибка: алгоритм не поддерживается\n";
        return 1;
    }

    if (mode.empty()) {
        std::cerr << "Ошибка: не указан режим работы\n";
        return 1;
    }

    if (mode != "encrypt" && mode != "decrypt" && mode != "generate-key") {
        std::cerr << "Ошибка: режим не поддерживается\n";
        return 1;
    }

    if (output_file.empty()) {
        std::cerr << "Ошибка: не указан выходной файл\n";
        return 1;
    }

    if (mode != "generate-key" && input_file.empty()) {
        std::cerr << "Ошибка: не указан входной файл\n";
        return 1;
    }

    std::string library_path = make_library_path(algorithm);
    LibraryHandle library = open_library(library_path);

    if (library == nullptr) {
        std::cerr << "Ошибка: не удалось загрузить библиотеку " << library_path << '\n';
        return 1;
    }

    using GetAlgorithmInfoFunc = const AlgorithmInfo* (*)();
    using GetOutputSizeFunc = size_t (*)(size_t, int);
    using EncryptFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);
    using DecryptFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);

    GetAlgorithmInfoFunc get_algorithm_info =
        reinterpret_cast<GetAlgorithmInfoFunc>(load_function(library, "get_algorithm_info"));
    GetOutputSizeFunc get_output_size =
        reinterpret_cast<GetOutputSizeFunc>(load_function(library, "get_output_size"));
    EncryptFunc encrypt_function =
        reinterpret_cast<EncryptFunc>(load_function(library, "encrypt"));
    DecryptFunc decrypt_function =
        reinterpret_cast<DecryptFunc>(load_function(library, "decrypt"));

    if (get_algorithm_info == nullptr ||
        get_output_size == nullptr ||
        encrypt_function == nullptr ||
        decrypt_function == nullptr) {

        std::cerr << "Ошибка: библиотека не содержит все необходимые функции\n";
        close_library(library);
        return 1;
    }

    const AlgorithmInfo* info = get_algorithm_info();

    if (info == nullptr) {
        std::cerr << "Ошибка: библиотека вернула пустую информацию об алгоритме\n";
        close_library(library);
        return 1;
    }

    if (mode == "generate-key") {
        if (info->key_size == 0) {
            std::cerr << "Ошибка: алгоритм " << algorithm << " не использует ключ\n";
            close_library(library);
            return 1;
        }

        std::vector<uint8_t> generated_key = generate_key(info->key_size);
        bool key_written = false;

        if (output_file == "-") {
            key_written = write_stdout(generated_key);
        }
        else {
            key_written = write_binary_file(output_file, generated_key);
        }

        if (!key_written) {
            std::cerr << "Ошибка: не удалось записать ключ\n";
            secure_clear(generated_key);
            close_library(library);
            return 1;
        }

        std::cerr << "Ключ успешно сгенерирован для " << info->algorithm_name << '\n';
        secure_clear(generated_key);
        close_library(library);
        return 0;
    }

    if (info->key_size > 0 && key_file.empty()) {
        std::cerr << "Ошибка: не указан файл ключа\n";
        close_library(library);
        return 1;
    }

    if (info->key_size == 0 && !key_file.empty()) {
        std::cerr << "Ошибка: алгоритм " << algorithm << " не использует ключ\n";
        close_library(library);
        return 1;
    }

    if (key_file == "-" && input_file == "-") {
        std::cerr << "Ошибка: нельзя одновременно читать ключ и входные данные из stdin\n";
        close_library(library);
        return 1;
    }

    std::vector<uint8_t> key_data;
    std::vector<uint8_t> input_data;

    if (info->key_size > 0) {
        if (key_file == "-") {
            if (!read_stdin(key_data)) {
                std::cerr << "Ошибка: не удалось прочитать ключ из stdin\n";
                close_library(library);
                return 1;
            }
        }
        else if (!read_binary_file(key_file, key_data)) {
            std::cerr << "Ошибка: не удалось открыть файл ключа\n";
            close_library(library);
            return 1;
        }

        if (key_data.size() != info->key_size) {
            std::cerr
                << "Ошибка: неверный размер ключа. Нужно "
                << info->key_size << " байт\n";
            secure_clear(key_data);
            close_library(library);
            return 1;
        }
    }

    if (input_file == "-") {
        if (!read_stdin(input_data)) {
            std::cerr << "Ошибка: не удалось прочитать входные данные из stdin\n";
            secure_clear(key_data);
            close_library(library);
            return 1;
        }
    }
    else if (!read_binary_file(input_file, input_data)) {
        std::cerr << "Ошибка: не удалось открыть входной файл\n";
        secure_clear(key_data);
        close_library(library);
        return 1;
    }

    int operation_type = mode == "decrypt" ? 1 : 0;
    size_t output_size = get_output_size(input_data.size(), operation_type);
    std::vector<uint8_t> output_data(output_size);

    uint8_t empty_output_byte = 0;

    ConstBuffer key_buffer {
        key_data.empty() ? nullptr : key_data.data(),
        key_data.size()
    };

    ConstBuffer input_buffer {
        input_data.empty() ? nullptr : input_data.data(),
        input_data.size()
    };

    MutBuffer output_buffer {
        output_data.empty() ? &empty_output_byte : output_data.data(),
        output_data.size()
    };

    int result = 0;

    if (mode == "encrypt") {
        result = encrypt_function(key_buffer, input_buffer, &output_buffer);
    }
    else {
        result = decrypt_function(key_buffer, input_buffer, &output_buffer);
    }

    if (result < 0) {
        std::cerr << "Ошибка: операция завершилась с кодом " << result << '\n';
        secure_clear(key_data);
        secure_clear(input_data);
        secure_clear(output_data);
        close_library(library);
        return 1;
    }

    if (result > 0 && static_cast<size_t>(result) <= output_data.size()) {
        output_data.resize(static_cast<size_t>(result));
    }
    else if (result == 0 && output_buffer.size <= output_data.size()) {
        output_data.resize(output_buffer.size);
    }

    bool output_written = false;

    if (output_file == "-") {
        output_written = write_stdout(output_data);
    }
    else {
        output_written = write_binary_file(output_file, output_data);
    }

    if (!output_written) {
        std::cerr << "Ошибка: не удалось записать выходные данные\n";
        close_library(library);
        secure_clear(key_data);
        secure_clear(input_data);
        secure_clear(output_data);
        return 1;
    }

    std::cerr << "Операция успешно выполнена алгоритмом " << info->algorithm_name << '\n';

    close_library(library);

    secure_clear(key_data);
    secure_clear(input_data);
    secure_clear(output_data);

    return 0;
}
