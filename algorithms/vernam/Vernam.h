#ifndef VERNAM_H
#define VERNAM_H

#include <cstddef>
#include <cstdint>

/**
 * @brief Структура для передачи константных данных
 */
struct ConstBuffer {
    const uint8_t* data;  ///< Указатель на данные
    size_t size;          ///< Размер данных в байтах
};

/**
 * @brief Структура для передачи изменяемых данных
 */
struct MutBuffer {
    uint8_t* data;        ///< Указатель на данные
    size_t size;          ///< Размер данных в байтах
};

/**
 * @brief Структура с метаинформацией об алгоритме
 */
struct AlgorithmInfo {
    const char* algorithm_name;  ///< Название алгоритма
    size_t key_size;             ///< Размер ключа в байтах (32 для Вернама)
};

/**
 * @brief Получить информацию об алгоритме
 * @return Указатель на структуру AlgorithmInfo
 */
extern "C" const AlgorithmInfo* get_algorithm_info();

/**
 * @brief Вычислить размер выходного буфера
 * @param input_size Размер входных данных
 * @param operation_type Тип операции (0 - шифрование, 1 - расшифрование)
 * @return Размер выходного буфера в байтах
 */
extern "C" size_t get_output_size(size_t input_size, int operation_type);

/**
 * @brief Зашифровать данные (XOR с ключом)
 * @param key Ключ шифрования (должен быть не пустым)
 * @param input Входные данные для шифрования
 * @param output Выходной буфер для результата
 * @return 0 - успех, отрицательное значение - ошибка
 * @retval -1 Ключ пустой или нулевой
 * @retval -2 Выходной буфер некорректен
 * @retval -3 Недостаточный размер выходного буфера
 */
extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);

/**
 * @brief Расшифровать данные (XOR с ключом)
 * @param key Ключ шифрования (должен быть не пустым)
 * @param input Входные данные для расшифрования
 * @param output Выходной буфер для результата
 * @return 0 - успех, отрицательное значение - ошибка
 * @retval -1 Ключ пустой или нулевой
 * @retval -2 Выходной буфер некорректен
 * @retval -3 Недостаточный размер выходного буфера
 */
extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);

#endif