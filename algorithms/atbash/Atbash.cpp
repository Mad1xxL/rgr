#include "atbash.h"

static const AlgorithmInfo ALGORITHM_INFO = {
    "atbash",
    0
};

static uint8_t atbash_transform(uint8_t ch) {
    // Английский алфавит (a-z)
    if (ch >= 'a' && ch <= 'z') {
        return 'z' - (ch - 'a');
    }
    // Английский алфавит (A-Z)
    if (ch >= 'A' && ch <= 'Z') {
        return 'Z' - (ch - 'A');
    }
    
    // Русский алфавит в Windows-1251 (а-я)
    static const uint8_t rus_lower[] = {
        'а','б','в','г','д','е','ё','ж','з','и','й','к','л','м',
        'н','о','п','р','с','т','у','ф','х','ц','ч','ш','щ','ъ',
        'ы','ь','э','ю','я'
    };
    static const uint8_t rus_upper[] = {
        'А','Б','В','Г','Д','Е','Ё','Ж','З','И','Й','К','Л','М',
        'Н','О','П','Р','С','Т','У','Ф','Х','Ц','Ч','Ш','Щ','Ъ',
        'Ы','Ь','Э','Ю','Я'
    };
    
    for (size_t i = 0; i < 33; ++i) {
        if (ch == rus_lower[i]) {
            return rus_lower[32 - i];
        }
        if (ch == rus_upper[i]) {
            return rus_upper[32 - i];
        }
    }
    
    // Остальные символы не изменяем
    return ch;
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    return &ALGORITHM_INFO;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    (void)operation_type;
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Проверка: Атбаш не использует ключ
    if (key.size != 0) {
        return -1;
    }
    
    // Проверка выходного буфера
    if (output == nullptr || output->data == nullptr) {
        return -2;
    }
    
    // Проверка размера буфера
    if (output->size < input.size) {
        return -3;
    }
    
    // Применяем преобразование Атбаш к каждому байту
    for (size_t i = 0; i < input.size; ++i) {
        output->data[i] = atbash_transform(input.data[i]);
    }
    
    output->size = input.size;
    return 0;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    // Для Атбаша шифрование и расшифрование идентичны
    return encrypt(key, input, output);
}