#include "Atbash.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {
    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;

    const AlgorithmInfo info = {
        "Atbash",
        0
    };

    uint8_t atbash_transform(uint8_t character) {
        // Английские строчные буквы: a-z
        if (character >= 'a' && character <= 'z') {
            return static_cast<uint8_t>(
                'z' - (character - 'a')
            );
        }

        // Английские заглавные буквы: A-Z
        if (character >= 'A' && character <= 'Z') {
            return static_cast<uint8_t>(
                'Z' - (character - 'A')
            );
        }

        // Русские заглавные буквы в Windows-1251:
        // А-Я, байты 0xC0-0xDF
        if (
            character >= 0xC0 &&
            character <= 0xDF
        ) {
            return static_cast<uint8_t>(
                0xDF - (character - 0xC0)
            );
        }

        // Русские строчные буквы в Windows-1251:
        // а-я, байты 0xE0-0xFF
        if(character >= 0xE0)
        {
            return static_cast<uint8_t>(
                0xFF - (character - 0xE0)
            );
        }

        // Ё и ё не изменяем,
        // потому что они не входят в диапазоны А-Я и а-я
        if (
            character == 0xA8 ||
            character == 0xB8
        ) {
            return character;
        }

        // Пробелы, цифры и знаки не изменяются
        return character;
    }

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

    int atbash_process(ConstBuffer input, MutBuffer* output) {
        std::cerr << "Atbash:\n";

        for (size_t i = 0; i < input.size; ++i) {
            uint8_t original = input.data[i];
            uint8_t transformed = atbash_transform(original);

            output->data[i] = transformed;

            std::cerr << i + 1 << ". ";
            show_symbol(original);
            std::cerr << " -> ";
            show_symbol(transformed);
            std::cerr << "\n";
        }

        return static_cast<int>(input.size);
    }
}

extern "C" const AlgorithmInfo*
get_algorithm_info() {
    return &info;
}

extern "C" size_t get_output_size(
    size_t input_size,
    int operation_type
) {
    (void)operation_type;

    return input_size;
}

extern "C" int encrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (key.size != 0) {
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

    return atbash_process(input, output);
}

extern "C" int decrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    // Полное дублирование логики encrypt
    // Atbash является обратным сам себе (инволютивный шифр)
    if (key.size != 0) {
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

    return atbash_process(input, output);
}