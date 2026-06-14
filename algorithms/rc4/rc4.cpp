#include "rc4.h"

static const AlgorithmInfo info = {
    "RC4",
    16
};

extern "C" const AlgorithmInfo* get_algorithm_info()
{
    return &info;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type)
{
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)
{
    return 0;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output)
{
    return 0;
}