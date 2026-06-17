#include "chacha20.h"

#include <vector>
#include <cstdint>

namespace {
    constexpr size_t CHACHA20_SECRET_KEY_SIZE = 32;
    constexpr size_t CHACHA20_NONCE_SIZE = 12;
    constexpr size_t CHACHA20_KEY_SIZE = CHACHA20_SECRET_KEY_SIZE + CHACHA20_NONCE_SIZE;
    constexpr size_t CHACHA20_BLOCK_SIZE = 64;

    constexpr int SUCCESS = 0;
    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;

    const AlgorithmInfo info = {
        "ChaCha20",
        CHACHA20_KEY_SIZE
    };
    

    uint32_t rotate_left(uint32_t value, int shift) {
        return (value << shift) | (value >> (32 - shift));
    }

    void quarter_round(std::vector<uint32_t>& state, size_t a, size_t b, size_t c, size_t d)    {
        state[a] += state[b];
        state[d] ^= state[a];
        state[d] = rotate_left(state[d], 16);

        state[c] += state[d];
        state[b] ^= state[c];
        state[b] = rotate_left(state[b], 12);

        state[a] += state[b];
        state[d] ^= state[a];
        state[d] = rotate_left(state[d], 8);

        state[c] += state[d];
        state[b] ^= state[c];
        state[b] = rotate_left(state[b], 7);
    }

    uint32_t load_32(const uint8_t* data)   {
        return static_cast<uint32_t>(data[0]) |
                (static_cast<uint32_t>(data[1]) << 8) |
                (static_cast<uint32_t>(data[2]) << 16) |
                (static_cast<uint32_t>(data[3]) << 24);
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info()    {
    return &info;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type)    {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)   {
    if (key.data == nullptr || key.size != CHACHA20_KEY_SIZE)    {
        return INVALID_KEY;
    }
    if (input.size > 0 && input.data == nullptr)    {
        return INVALID_INPUT;
    }

    if (output == nullptr || output->data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output->size < input.size)    {
        return INVALID_OUTPUT;
    }

    return SUCCESS;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)   {
    if (key.data == nullptr || key.size != CHACHA20_KEY_SIZE)    {
        return INVALID_KEY;
    }
    if (input.size > 0 && input.data == nullptr)    {
        return INVALID_INPUT;
    }

    if (output == nullptr || output->data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output->size < input.size)    {
        return INVALID_OUTPUT;
    }

    return SUCCESS;
}