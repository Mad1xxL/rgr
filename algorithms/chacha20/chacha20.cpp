#include "chacha20.h"

#include <cstdint>

namespace {
    constexpr size_t CHACHA20_KEY_SIZE = 32;

    constexpr int SUCCESS = 0;
    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;

    const AlgorithmInfo info = {
        "ChaCha20",
        CHACHA20_KEY_SIZE
    };
    
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