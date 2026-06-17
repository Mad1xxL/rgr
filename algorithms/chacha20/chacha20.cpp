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

    const AlgorithmInfo info = {"ChaCha20", CHACHA20_KEY_SIZE};
    

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

    std::vector<uint32_t> create_initial_state(const uint8_t* key, const uint8_t* nonce)    {
        std::vector<uint32_t> state(16);

        // Константы ChaCha20 ("expand 32-byte k")
        state[0] = 0x61707865;
        state[1] = 0x3320646e;
        state[2] = 0x79622d32;
        state[3] = 0x6b206574;

        for (size_t i = 0 ; i < 8 ; ++i)    {
            state[4 + i] = load_32(key + i * 4);
        }

        state[12] = 0;

        state[13] = load_32(nonce);
        state[14] = load_32(nonce + 4);
        state[15] = load_32(nonce + 8);

        return state;
    }

    void save_32(uint32_t value, uint8_t* output)   {
        output[0] = static_cast<uint8_t>(value);
        output[1] = static_cast<uint8_t>(value >> 8);
        output[2] = static_cast<uint8_t>(value >> 16);
        output[3] = static_cast<uint8_t>(value >> 24);
    }

    void generate_block(std::vector<uint32_t>& state, uint8_t* output)
{
    std::vector<uint32_t> working_state = state;

    for (size_t i = 0; i < 10; ++i)
    {
        quarter_round(working_state, 0, 4, 8, 12);
        quarter_round(working_state, 1, 5, 9, 13);
        quarter_round(working_state, 2, 6, 10, 14);
        quarter_round(working_state, 3, 7, 11, 15);

        quarter_round(working_state, 0, 5, 10, 15);
        quarter_round(working_state, 1, 6, 11, 12);
        quarter_round(working_state, 2, 7, 8, 13);
        quarter_round(working_state, 3, 4, 9, 14);
    }

    for (size_t i = 0; i < 16; ++i)
    {
        working_state[i] += state[i];
        save_32(working_state[i], output + i * 4);
    }

    ++state[12];
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