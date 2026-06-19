#include "Vernam.h"

static const AlgorithmInfo ALGORITHM_INFO = {
    "vernam",
    32
};

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &ALGORITHM_INFO;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size == 0 || key.data == nullptr) {
        return -1;
    }
    
    if (output == nullptr || output->data == nullptr) {
        return -2;
    }
    
    if (output->size < input.size) {
        return -3;
    }
    
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = input.data[i] ^ key.data[i % key.size];
    }
    
    output->size = input.size;
    return 0;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    return encrypt(key, input, output);
}