#include "rc4.h"

#include <vector> 
#include <cstdint>
#include <algorithm>

namespace {
    constexpr size_t RC4_KEY_SIZE = 16;

    constexpr int SUCCSESS = 0;
    constexpr int INVALID_KEY = -1;
    constexpr int INVALID_INPUT = -2;
    constexpr int INVALID_OUTPUT = -3;
    
    //Перемешивание массива state(внутреннего состояния RC4) используя ключ
    void ksa(const uint8_t* key, size_t key_size, std::vector<uint8_t>& state)  {
        state.resize(256);

        for (size_t i = 0 ; i < 256 ; ++i)  {
            state[i] = static_cast<uint8_t>(i);
        }

        size_t j = 0;
        for (size_t i = 0 ; i < 256 ; ++i)  {
            j = (j + state[i] + key[i % key_size]) % 256;
            std::swap(state[i], state[j]);
        }
    }

    void rc4_process(ConstBuffer key, ConstBuffer input, MutBuffer* output) {

    }
}

static const AlgorithmInfo info = {
    "RC4",
    RC4_KEY_SIZE
};

extern "C" const AlgorithmInfo* get_algorithm_info()    {
    return &info;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type)    {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)   {
    if (key.data == nullptr || key.size != RC4_KEY_SIZE)    {
        return INVALID_KEY;
    }
    if (input.size > 0 && input.data == nullptr)    {
        return INVALID_INPUT;
    }

    if (output == nullptr || output -> data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output -> size < input.size)    {
        return INVALID_OUTPUT;
    }

    return SUCCSESS;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)   {
    if (key.data == nullptr || key.size != RC4_KEY_SIZE)    {
        return INVALID_KEY;
    }
    if (input.size > 0 && input.data == nullptr)    {
        return INVALID_INPUT;
    }

    if (output == nullptr || output -> data == nullptr) {
        return INVALID_OUTPUT;
    }

    if (output -> size < input.size)    {
        return INVALID_OUTPUT;
    }

    return SUCCSESS;
}