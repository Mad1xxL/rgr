#include "Vernam.h"

static const AlgorithmInfo ALGORITHM_INFO = {
    "vernam",
    32  // Рекомендуемая длина ключа 32 байта (256 бит)
};

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &ALGORITHM_INFO;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Проверка ключа
    if (key.size == 0 || key.data == nullptr) {
        return -1;  // Ошибка: ключ не может быть пустым
    }
    
    // Проверка выходного буфера
    if (output == nullptr || output->data == nullptr) {
        return -2;  // Ошибка: некорректный выходной буфер
    }
    
    // Проверка размера буфера
    if (output->size < input.size) {
        return -3;  // Ошибка: недостаточный размер буфера
    }
    
    // Шифрование Вернама (XOR с циклическим использованием ключа)
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = input.data[i] ^ key.data[i % key.size];
    }
    
    output->size = input.size;
    return 0;  // Успех
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Для XOR шифрование и расшифрование идентичны
    return encrypt(key, input, output);
}