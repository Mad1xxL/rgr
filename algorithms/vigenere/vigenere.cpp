#include "vigenere.h"

#include <cstddef>
#include <cstdint>

namespace {
    constexpr size_t VIGENERE_KEY_SIZE = 16;

    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;

    const AlgorithmInfo info = {
        "Vigenere-256",
        VIGENERE_KEY_SIZE
    };

    void vigenere_process(
        ConstBuffer key,
        ConstBuffer input,
        MutBuffer* output,
        bool encrypt_mode
    ) {
        for (size_t i = 0; i < input.size; ++i) {
            // Если данные длиннее ключа,
            // ключ начинает использоваться сначала.
            uint8_t key_byte =
                key.data[i % key.size];

            if (encrypt_mode) {
                // Шифрование:
                // байт данных + байт ключа.
                output->data[i] =
                    static_cast<uint8_t>(
                        input.data[i] + key_byte
                    );
            }
            else {
                // Расшифровка:
                // зашифрованный байт - байт ключа.
                output->data[i] =
                    static_cast<uint8_t>(
                        input.data[i] - key_byte
                    );
            }
        }
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
    // Размер результата не меняется.
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (key.data == nullptr ||
        key.size != VIGENERE_KEY_SIZE) {

        return INVALID_KEY;
    }

    if (input.size > 0 &&
        input.data == nullptr) {

        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (input.size > 0 &&
        output->data == nullptr) {

        return INVALID_OUTPUT;
    }

    if (output->size < input.size) {
        return INVALID_OUTPUT;
    }

    vigenere_process(
        key,
        input,
        output,
        true
    );

    return static_cast<int>(input.size);
}

extern "C" int decrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (key.data == nullptr ||
        key.size != VIGENERE_KEY_SIZE) {

        return INVALID_KEY;
    }

    if (input.size > 0 &&
        input.data == nullptr) {

        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (input.size > 0 &&
        output->data == nullptr) {

        return INVALID_OUTPUT;
    }

    if (output->size < input.size) {
        return INVALID_OUTPUT;
    }

    vigenere_process(
        key,
        input,
        output,
        false
    );

    return static_cast<int>(input.size);
}