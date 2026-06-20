#include "Vernam.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

static const AlgorithmInfo ALGORITHM_INFO = {
    "vernam",
    32
};

namespace {
    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;

    void show_symbol(uint8_t character) {
        switch (character) {
            case '\n':
                std::cerr << "'\\n'";
                break;
            case '\t':
                std::cerr << "'\\t'";
                break;
            default:
                std::cerr << "'" << static_cast<char>(character) << "'";
                break;
        }
    }

    int vernam_process(
        ConstBuffer key,
        ConstBuffer input,
        MutBuffer* output
    ) {
        std::cerr << "Vernam:\n";

        for (size_t i = 0; i < input.size; ++i) {
            uint8_t original = input.data[i];
            uint8_t key_character = key.data[i % key.size];
            uint8_t transformed = original ^ key_character;

            output->data[i] = transformed;

            std::cerr << i + 1 << ". ";
            show_symbol(original);
            std::cerr << " ^ ";
            show_symbol(key_character);
            std::cerr << " = ";
            show_symbol(transformed);
            std::cerr << "\n";
        }

        output->size = input.size;
        return static_cast<int>(input.size);
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &ALGORITHM_INFO;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size == 0 || key.data == nullptr) {
        return INVALID_KEY;
    }

    if (input.size > 0 && input.data == nullptr) {
        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (input.size > 0 && output->data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output->size < input.size) {
        return INVALID_OUTPUT;
    }

    return vernam_process(key, input, output);
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Полное дублирование логики encrypt
    // Для шифра Вернама (XOR) шифрование и дешифрование идентичны
    if (key.size == 0 || key.data == nullptr) {
        return INVALID_KEY;
    }

    if (input.size > 0 && input.data == nullptr) {
        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (input.size > 0 && output->data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output->size < input.size) {
        return INVALID_OUTPUT;
    }

    return vernam_process(key, input, output);
}