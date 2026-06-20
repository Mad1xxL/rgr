#include "elgamal.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace {

    constexpr uint32_t G = 3;
    constexpr uint32_t MAX_P = 500000;

    constexpr size_t ELGAMAL_KEY_SIZE = 4;
    constexpr size_t ENCRYPTED_BYTE_SIZE = 8;

    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;
    constexpr int INVALID_CIPHERTEXT = -4;

    const AlgorithmInfo info = {
        "ElGamal",
        ELGAMAL_KEY_SIZE
    };

    // Сборка числа uint32_t из четырёх байтов
    uint32_t load_32(const uint8_t* data) {
        return static_cast<uint32_t>(data[0]) |
               (static_cast<uint32_t>(data[1]) << 8) |
               (static_cast<uint32_t>(data[2]) << 16) |
               (static_cast<uint32_t>(data[3]) << 24);
    }

    // Разделение числа uint32_t на четыре байта
    void save_32(uint32_t value, uint8_t* output) {
        output[0] = static_cast<uint8_t>(value);
        output[1] = static_cast<uint8_t>(value >> 8);
        output[2] = static_cast<uint8_t>(value >> 16);
        output[3] = static_cast<uint8_t>(value >> 24);
    }

    std::vector<uint32_t> sieve() {
        std::vector<bool> is_prime(MAX_P + 1, true);
        is_prime[0] = false;
        is_prime[1] = false;
        for (uint32_t i = 2; i * i <= MAX_P; ++i) {
            if (is_prime[i]) {
                for (
                    uint32_t j = i * i;
                    j <= MAX_P;
                    j += i
                ) {
                    is_prime[j] = false;
                }
            }
        }

        std::vector<uint32_t> primes;
        for (uint32_t i = 257; i <= MAX_P; ++i) {
            if (is_prime[i]) {
                primes.push_back(i);
            }
        }

        return primes;
    }

    uint32_t generate_p(ConstBuffer key) {
        static const std::vector<uint32_t> primes = sieve();
        uint32_t key_number = load_32(key.data);
        size_t index = key_number % primes.size();
        return primes[index];
    }

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

    // Получение закрытого ключа x
    uint32_t get_private_key(
        ConstBuffer key,
        uint32_t p
    ) {
        uint32_t value = load_32(key.data);

        return value % (p - 2) + 1;
    }

    // Генерация случайного числа k
    uint32_t generate_k(uint32_t p) {
        static std::random_device random_device;

        static std::mt19937 generator(
            random_device()
        );

        std::uniform_int_distribution<uint32_t> distribution(
            1,
            p - 2
        );

        return distribution(generator);
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &info;
}

extern "C" size_t get_output_size(
    size_t input_size,
    int operation_type
) {
    if (operation_type == 0) {
        return input_size * ENCRYPTED_BYTE_SIZE;
    }

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
    if (
        key.data == nullptr ||
        key.size != ELGAMAL_KEY_SIZE
    ) {
return INVALID_KEY;
    }

    if (
        input.size > 0 &&
        input.data == nullptr
    ) {
        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    size_t output_size =
        input.size * ENCRYPTED_BYTE_SIZE;

    if (
        output_size > 0 &&
        output->data == nullptr
    ) {
        return INVALID_OUTPUT;
    }

    if (output->size < output_size) {
        return INVALID_OUTPUT;
    }

    uint32_t p = generate_p(key);

    // x — закрытый ключ
    uint32_t x = get_private_key(key, p);

    // y — открытый ключ
    uint32_t y = mod_pow(G, x, p);

    for (size_t i = 0; i < input.size; ++i) {
        // Случайное число для текущего байта
        uint32_t k = generate_k(p);
        uint32_t c1 = mod_pow(G, k, p);
        uint32_t shared_secret =
            mod_pow(y, k, p);
        uint32_t c2 = static_cast<uint32_t>(
            (
                static_cast<uint64_t>(
                    input.data[i]
                ) * shared_secret
            ) % p
        );

        uint8_t* encrypted =
            output->data +
            i * ENCRYPTED_BYTE_SIZE;

        save_32(c1, encrypted);
        save_32(c2, encrypted + 4);
    }

    return static_cast<int>(output_size);
}

extern "C" int decrypt(
    ConstBuffer key,
    ConstBuffer input,
    MutBuffer* output
) {
    if (
        key.data == nullptr ||
        key.size != ELGAMAL_KEY_SIZE
    ) {
        return INVALID_KEY;
    }

    if (
        input.size > 0 &&
        input.data == nullptr
    ) {
        return INVALID_INPUT;
    }

    if (output == nullptr) {
        return INVALID_OUTPUT;
    }

    if (
        input.size % ENCRYPTED_BYTE_SIZE != 0
    ) {
        return INVALID_CIPHERTEXT;
    }

    size_t output_size =
        input.size / ENCRYPTED_BYTE_SIZE;

    if (
        output_size > 0 &&
        output->data == nullptr
    ) {
        return INVALID_OUTPUT;
    }

    if (output->size < output_size) {
        return INVALID_OUTPUT;
    }

    uint32_t p = generate_p(key);
    uint32_t x = get_private_key(key, p);

    for (size_t i = 0; i < output_size; ++i) {
        const uint8_t* encrypted =
            input.data +
            i * ENCRYPTED_BYTE_SIZE;

        // Первые четыре байта — C1
        uint32_t c1 = load_32(encrypted);

        // Следующие четыре байта — C2
        uint32_t c2 = load_32(encrypted + 4);

        if (
            c1 == 0 ||
            c1 >= p ||
            c2 >= p
        ) {
            return INVALID_CIPHERTEXT;
        }

        // Общий секрет = C1^x mod P
        uint32_t shared_secret =
            mod_pow(c1, x, p);

        // Обратное число для общего секрета
        uint32_t inverse = mod_pow(
            shared_secret,
            p - 2,
            p
        );

        // Восстановление исходного байта
        uint32_t message =
            static_cast<uint32_t>(
                (
                    static_cast<uint64_t>(c2) *
                    inverse
                ) % p
            );

        // Результат должен помещаться в один байт
        if (message > 255) {
            return INVALID_CIPHERTEXT;
        }

        output->data[i] =
            static_cast<uint8_t>(message);
    }

    return static_cast<int>(output_size);
}