#include "elgamal.h"

#include <cstddef>
#include <cstdint>
#include <random>

namespace {
    // Общие параметры ElGamal.
    constexpr uint32_t P = 65537;
    constexpr uint32_t G = 3;

    constexpr size_t ELGAMAL_KEY_SIZE = 4;

    // Один байт превращается в два числа:
    // C1 — 4 байта, C2 — 4 байта.
    constexpr size_t ENCRYPTED_BYTE_SIZE = 8;

    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;
    constexpr int INVALID_CIPHERTEXT = -4;

    const AlgorithmInfo info = {
        "ElGamal",
        ELGAMAL_KEY_SIZE
    };

    // Собирает число uint32_t из четырёх байтов.
    uint32_t load_32(const uint8_t* data) {
        return static_cast<uint32_t>(data[0]) |
               (static_cast<uint32_t>(data[1]) << 8) |
               (static_cast<uint32_t>(data[2]) << 16) |
               (static_cast<uint32_t>(data[3]) << 24);
    }

    // Разделяет число uint32_t на четыре байта.
    void save_32(uint32_t value, uint8_t* output) {
        output[0] = static_cast<uint8_t>(value);
        output[1] = static_cast<uint8_t>(value >> 8);
        output[2] = static_cast<uint8_t>(value >> 16);
        output[3] = static_cast<uint8_t>(value >> 24);
    }

    // Вычисляет:
    // base^exponent mod modulus.
    uint32_t mod_pow(
        uint32_t base,
        uint32_t exponent,
        uint32_t modulus
    ) {
        uint64_t result = 1;
        uint64_t current = base % modulus;

        while (exponent > 0) {
            if (exponent % 2 == 1) {
                result = (result * current) % modulus;
            }

            current = (current * current) % modulus;
            exponent /= 2;
        }

        return static_cast<uint32_t>(result);
    }

    // Получает закрытый ключ x из четырёх байтов.
    uint32_t get_private_key(ConstBuffer key) {
        uint32_t value = load_32(key.data);

        return value % (P - 2) + 1;
    }

    // Генерирует случайное число k от 1 до P - 2.
    uint32_t generate_k() {
        static std::random_device random_device;

        static std::uniform_int_distribution<uint32_t> distribution(
            1,
            P - 2
        );

        return distribution(random_device);
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &info;
}

extern "C" size_t get_output_size(
    size_t input_size,
    int operation_type
) {
    // 0 — шифрование.
    if (operation_type == 0) {
        return input_size * ENCRYPTED_BYTE_SIZE;
    }

    // Расшифровка.
    if (input_size % ENCRYPTED_BYTE_SIZE != 0) {
        return 0;
    }

    return input_size / ENCRYPTED_BYTE_SIZE;
}

extern "C" int encrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (key.data == nullptr ||
        key.size != ELGAMAL_KEY_SIZE) {

        return INVALID_KEY;
    }

    if (input.size > 0 &&
        input.data == nullptr) {

        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    size_t output_size =
        input.size * ENCRYPTED_BYTE_SIZE;

    if (output_size > 0 &&
        output->data == nullptr) {

        return INVALID_OUTPUT;
    }

    if (output->size < output_size) {
        return INVALID_OUTPUT;
    }

    // x — закрытый ключ.
    uint32_t x = get_private_key(key);

    // y = G^x mod P — открытый ключ.
    uint32_t y = mod_pow(G, x, P);

    for (size_t i = 0; i < input.size; ++i) {
        // Случайное число для текущего байта.
        uint32_t k = generate_k();

        // C1 = G^k mod P.
        uint32_t c1 = mod_pow(G, k, P);

        // Общий секрет y^k mod P.
        uint32_t shared_secret =
            mod_pow(y, k, P);

        // C2 = сообщение * общий секрет mod P.
        uint32_t c2 = static_cast<uint32_t>(
            (
                static_cast<uint64_t>(input.data[i]) *
                shared_secret
            ) % P
        );

        // Для каждого байта выделено 8 байт.
        uint8_t* encrypted =
            output->data + i * ENCRYPTED_BYTE_SIZE;

        save_32(c1, encrypted);
        save_32(c2,
encrypted + 4);
    }

    return static_cast<int>(output_size);
}

extern "C" int decrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (key.data == nullptr ||
        key.size != ELGAMAL_KEY_SIZE) {

        return INVALID_KEY;
    }

    if (input.size > 0 &&
        input.data == nullptr) {

        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (input.size % ENCRYPTED_BYTE_SIZE != 0) {
        return INVALID_CIPHERTEXT;
    }

    size_t output_size =
        input.size / ENCRYPTED_BYTE_SIZE;

    if (output_size > 0 &&
        output->data == nullptr) {

        return INVALID_OUTPUT;
    }

    if (output->size < output_size) {
        return INVALID_OUTPUT;
    }

    // Получаем тот же закрытый ключ.
    uint32_t x = get_private_key(key);

    for (size_t i = 0; i < output_size; ++i) {
        const uint8_t* encrypted =
            input.data + i * ENCRYPTED_BYTE_SIZE;

        // Читаем C1 и C2.
        uint32_t c1 = load_32(encrypted);
        uint32_t c2 = load_32(encrypted + 4);

        if (c1 == 0 ||
            c1 >= P ||
            c2 >= P) {

            return INVALID_CIPHERTEXT;
        }

        // Общий секрет C1^x mod P.
        uint32_t shared_secret =
            mod_pow(c1, x, P);

        // Обратное число по модулю P.
        uint32_t inverse =
            mod_pow(shared_secret, P - 2, P);

        // Восстанавливаем исходный байт.
        uint32_t message = static_cast<uint32_t>(
            (
                static_cast<uint64_t>(c2) *
                inverse
            ) % P
        );

        // Исходные данные должны помещаться в один байт.
        if (message > 255) {
            return INVALID_CIPHERTEXT;
        }

        output->data[i] =
            static_cast<uint8_t>(message);
    }

    return static_cast<int>(output_size);
}