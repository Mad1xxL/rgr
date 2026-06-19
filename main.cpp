#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <random>
#include <map>
#include <cstring>

#include "../include/crypto_api.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

static const map<string, string> ALGORITHM_LIBRARIES = {
    {"atbash", "atbash"},
    {"vernam", "vernam"}
};

#ifdef _WIN32
using LibraryHandle = HMODULE;
#else
using LibraryHandle = void*;
#endif

bool read_binary_file(const string& path, vector<uint8_t>& data) {
    ifstream file(path, ios::binary);
    if (!file) return false;
    file.seekg(0, ios::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, ios::beg);
    data.resize(size);
    if (size > 0) {
        file.read(reinterpret_cast<char*>(data.data()), size);
    }
    return true;
}

bool write_binary_file(const string& path, const vector<uint8_t>& data) {
    ofstream file(path, ios::binary);
    if (!file) return false;
    if (!data.empty()) {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
    }
    return true;
}

vector<uint8_t> generate_key(size_t size) {
    vector<uint8_t> key(size);
    random_device rd;
    for (size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<uint8_t>(rd());
    }
    return key;
}

void secure_zero_memory(vector<uint8_t>& data) {
    if (data.empty()) return;
    volatile uint8_t* ptr = data.data();
    for (size_t i = 0; i < data.size(); ++i) {
        ptr[i] = 0;
    }
}

void print_help() {
    cout << "Cryptum - Multi-Algorithm Cryptotool\n"
         << "Usage:\n"
         << "  cryptum [options]\n\n"
         << "Options:\n"
         << "  --help, -h                       Show this help\n"
         << "  -a, --algorithm <algo>           atbash | vernam\n"
         << "  -m, --mode <mode>                encrypt | decrypt | generate-key\n"
         << "  -k, --key <file>                 Path to key file\n"
         << "  -i, --input <file>               Path to input file\n"
         << "  -o, --output <file>              Path to output file\n"
         << "\nExamples:\n"
         << "  cryptum -a vernam -m generate-key -o key.bin\n"
         << "  cryptum -a atbash -m encrypt -i input.txt -o output.txt\n"
         << "  cryptum -a vernam -m decrypt -k key.bin -i encrypted.bin -o decrypted.txt\n";
}

LibraryHandle open_library(const string& path) {
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

string get_library_path(const string& algorithm) {
    auto it = ALGORITHM_LIBRARIES.find(algorithm);
    if (it == ALGORITHM_LIBRARIES.end()) return "";
    string base = it->second;
#ifdef _WIN32
    return "bin/" + base + "/" + base + ".dll";
#elif __APPLE__
    return "bin/" + base + "/lib" + base + ".dylib";
#else
    return "bin/" + base + "/lib" + base + ".so";
#endif
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    
    if (argc == 1) {
        print_help();
        return 0;
    }
    
    string first_arg = argv[1];
    if (first_arg == "--help" || first_arg == "-h") {
        print_help();
        return 0;
    }
    
    string algorithm, mode, key_file, input_file, output_file;
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-a" || arg == "--algorithm") && i + 1 < argc) {
            algorithm = argv[++i];
        } else if ((arg == "-m" || arg == "--mode") && i + 1 < argc) {
            mode = argv[++i];
        } else if ((arg == "-k" || arg == "--key") && i + 1 < argc) {
            key_file = argv[++i];
        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            input_file = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output_file = argv[++i];
        }
    }
    
    if (algorithm.empty()) {
        cerr << "Error: algorithm not specified\n";
        return 1;
    }
    if (mode.empty()) {
        cerr << "Error: mode not specified\n";
        return 1;
    }
    if (ALGORITHM_LIBRARIES.find(algorithm) == ALGORITHM_LIBRARIES.end()) {
        cerr << "Error: unsupported algorithm '" << algorithm << "'\n";
        return 1;
    }
    if (mode != "encrypt" && mode != "decrypt" && mode != "generate-key") {
        cerr << "Error: unsupported mode '" << mode << "'\n";
        return 1;
    }
    if (mode != "generate-key" && key_file.empty()) {
        cerr << "Error: key file not specified\n";
        return 1;
    }
    if (mode != "generate-key" && input_file.empty()) {
        cerr << "Error: input file not specified\n";
        return 1;
    }
    if (output_file.empty()) {
        cerr << "Error: output file not specified\n";
        return 1;
    }
    
    cout << "Algorithm: " << algorithm << '\n';
    cout << "Mode: " << mode << '\n';
    
    string lib_path = get_library_path(algorithm);
    LibraryHandle lib = open_library(lib_path);
    if (lib == nullptr) {
        cerr << "Error: failed to load library '" << lib_path << "'\n";
        return 1;
    }
    
    using GetInfoFunc = const AlgorithmInfo* (*)();
    using GetSizeFunc = size_t (*)(size_t, int);
    using CryptoFunc = int (*)(ConstBuffer, ConstBuffer, MutBuffer*);
    
    auto get_info = reinterpret_cast<GetInfoFunc>(load_function(lib, "get_algorithm_info"));
    auto get_size = reinterpret_cast<GetSizeFunc>(load_function(lib, "get_output_size"));
    auto encrypt_func = reinterpret_cast<CryptoFunc>(load_function(lib, "encrypt"));
    auto decrypt_func = reinterpret_cast<CryptoFunc>(load_function(lib, "decrypt"));
    
    if (get_info == nullptr || get_size == nullptr || 
        encrypt_func == nullptr || decrypt_func == nullptr) {
        cerr << "Error: failed to load functions from library\n";
        close_library(lib);
        return 1;
    }
    
    const AlgorithmInfo* info = get_info();
    cout << "Algorithm name: " << info->algorithm_name << '\n';
    cout << "Key size: " << info->key_size << " bytes\n";
    
    if (mode == "generate-key") {
        if (info->key_size == 0) {
            cerr << "Error: algorithm does not support key generation\n";
            close_library(lib);
            return 1;
        }
        vector<uint8_t> key = generate_key(info->key_size);
        if (!write_binary_file(output_file, key)) {
            cerr << "Error: failed to write key file\n";
            close_library(lib);
            return 1;
        }
        secure_zero_memory(key);
        cout << "Key generated and saved to '" << output_file << "'\n";
        close_library(lib);
        return 0;
    }
    
    vector<uint8_t> key_data;
    if (!read_binary_file(key_file, key_data)) {
        cerr << "Error: failed to open key file '" << key_file << "'\n";
        close_library(lib);
        return 1;
    }
    
    if (info->key_size > 0 && key_data.size() != info->key_size) {
        cerr << "Error: key size (" << key_data.size() 
             << " bytes) does not match expected (" << info->key_size << " bytes)\n";
        close_library(lib);
        return 1;
    }
    
    vector<uint8_t> input_data;
    if (!read_binary_file(input_file, input_data)) {
        cerr << "Error: failed to open input file '" << input_file << "'\n";
        close_library(lib);
        return 1;
    }
    
    cout << "Input size: " << input_data.size() << " bytes\n";
    
    int op_type = (mode == "decrypt") ? 1 : 0;
    size_t output_size = get_size(input_data.size(), op_type);
    vector<uint8_t> output_data(output_size);
    
    ConstBuffer key_buf = {key_data.data(), key_data.size()};
    ConstBuffer input_buf = {input_data.data(), input_data.size()};
    MutBuffer output_buf = {output_data.data(), output_data.size()};
    
    int result = 0;
    if (mode == "encrypt") {
        result = encrypt_func(key_buf, input_buf, &output_buf);
    } else {
        result = decrypt_func(key_buf, input_buf, &output_buf);
    }
    
    if (result < 0) {
        cerr << "Error: operation failed with code " << result << '\n';
        close_library(lib);
        secure_zero_memory(key_data);
        secure_zero_memory(input_data);
        return 1;
    }
    
    output_data.resize(output_buf.size);
    if (!write_binary_file(output_file, output_data)) {
        cerr << "Error: failed to write output file '" << output_file << "'\n";
        close_library(lib);
        return 1;
    }
    
    cout << "Operation completed successfully\n";
    cout << "Output saved to '" << output_file << "'\n";
    
    secure_zero_memory(key_data);
    secure_zero_memory(input_data);
    secure_zero_memory(output_data);
    
    close_library(lib);
    return 0;
}