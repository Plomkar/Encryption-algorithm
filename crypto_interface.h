#ifndef CRYPTO_INTERFACE_H
#define CRYPTO_INTERFACE_H

#include <vector>
#include <cstdint>

// Используем extern "C", чтобы избежать искажения (mangling) имен компилятором C++
extern "C" {
    // Функция шифрования байтового массива
    // data - указатель на данные, size - размер данных, key - ключ, key_size - размер ключа
    uint8_t* encrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size);

    // Функция дешифрования байтового массива
    uint8_t* decrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size);

    // Функция освобождения памяти, выделенной внутри библиотеки
    void free_buffer(uint8_t* buffer);
}

#endif