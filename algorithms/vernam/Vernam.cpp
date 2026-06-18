#include "vernam.h"

/**
 * @brief Метаинформация об алгоритме Вернама
 */
static const AlgorithmInfo ALGORITHM_INFO = {
    "vernam",
    32  // Рекомендуемая длина ключа 32 байта (256 бит)
};

/**
 * @brief Получить информацию об алгоритме
 * @return Указатель на структуру AlgorithmInfo
 */
extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &ALGORITHM_INFO;
}

/**
 * @brief Вычислить размер выходного буфера
 * @param input_size Размер входных данных
 * @param operation_type Тип операции (0 - шифрование, 1 - расшифрование)
 * @return Размер выходного буфера в байтах
 */
extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    (void)operation_type;  // Для Вернама размер не меняется
    return input_size;
}

/**
 * @brief Зашифровать данные с помощью шифра Вернама (XOR)
 * @param key Ключ шифрования
 * @param input Входные данные
 * @param output Выходной буфер
 * @return 0 - успех, отрицательное значение - ошибка
 */
extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Проверка: ключ не должен быть пустым
    if (key.size == 0 || key.data == nullptr) {
        return -1;  // Ошибка: ключ не может быть пустым
    }
    
    // Проверка: выходной буфер должен существовать
    if (output == nullptr || output->data == nullptr) {
        return -2;  // Ошибка: некорректный выходной буфер
    }
    
    // Проверка: размер выходного буфера должен быть достаточным
    if (output->size < input.size) {
        return -3;  // Ошибка: недостаточный размер буфера
    }
    
    // Шифрование Вернама (побайтовый XOR с циклическим использованием ключа)
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = input.data[i] ^ key.data[i % key.size];
    }
    
    // Обновляем размер выходных данных
    output->size = input.size;
    
    return 0;  // Успех
}

/**
 * @brief Расшифровать данные с помощью шифра Вернама (XOR)
 * @param key Ключ шифрования
 * @param input Входные данные
 * @param output Выходной буфер
 * @return 0 - успех, отрицательное значение - ошибка
 */
extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Для XOR шифрование и расшифрование идентичны
    return encrypt(key, input, output);
}