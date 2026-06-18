#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// Английский алфавит
const string ENG_LOWER = "abcdefghijklmnopqrstuvwxyz";
const string ENG_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Русский алфавит
const string RUS_LOWER = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
const string RUS_UPPER = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";

string atbash_encrypt(const string& text) {
    string result = text;
    
    for (char& ch : result) {
        // Проверяем английские буквы
        if (isalpha(static_cast<unsigned char>(ch))) {
            // Английский
            if (ch >= 'a' && ch <= 'z') {
                ch = ENG_LOWER[25 - (ch - 'a')];
            }
            else if (ch >= 'A' && ch <= 'Z') {
                ch = ENG_UPPER[25 - (ch - 'A')];
            }
            // Русский
            else {
                size_t pos = RUS_LOWER.find(ch);
                if (pos != string::npos) {
                    ch = RUS_LOWER[RUS_LOWER.length() - 1 - pos];
                }
                else {
                    pos = RUS_UPPER.find(ch);
                    if (pos != string::npos) {
                        ch = RUS_UPPER[RUS_UPPER.length() - 1 - pos];
                    }
                }
            }
        }
        // Остальные символы (цифры, знаки препинания) не изменяем
    }
    
    return result;
}

// Дешифрование - то же самое, что и шифрование
string atbash_decrypt(const string& text) {
    return atbash_encrypt(text);
}

// Версия для работы с wstring (Unicode)
wstring atbash_encrypt_wide(const wstring& text) {
    const wstring ENG_LOWER_W = L"abcdefghijklmnopqrstuvwxyz";
    const wstring ENG_UPPER_W = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const wstring RUS_LOWER_W = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const wstring RUS_UPPER_W = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    
    wstring result = text;
    
    for (wchar_t& ch : result) {
        // Английский
        size_t pos = ENG_LOWER_W.find(ch);
        if (pos != wstring::npos) {
            ch = ENG_LOWER_W[ENG_LOWER_W.length() - 1 - pos];
            continue;
        }
        
        pos = ENG_UPPER_W.find(ch);
        if (pos != wstring::npos) {
            ch = ENG_UPPER_W[ENG_UPPER_W.length() - 1 - pos];
            continue;
        }
        
        // Русский
        pos = RUS_LOWER_W.find(ch);
        if (pos != wstring::npos) {
            ch = RUS_LOWER_W[RUS_LOWER_W.length() - 1 - pos];
            continue;
        }
        
        pos = RUS_UPPER_W.find(ch);
        if (pos != wstring::npos) {
            ch = RUS_UPPER_W[RUS_UPPER_W.length() - 1 - pos];
            continue;
        }
    }
    
    return result;
}

wstring atbash_decrypt_wide(const wstring& text) {
    return atbash_encrypt_wide(text);
}

// Утилитарная функция для вывода
void print_result(const string& label, const string& text) {
    cout << label << ": " << text << endl;
}