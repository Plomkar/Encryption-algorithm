#include "crypto_interface.h"
#include <cstdlib>

extern "C" {
    // Шифрование Атбаш: инверсия байта
    uint8_t* encrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size) {
        (void)key; (void)key_size; // Игнорируем неиспользуемые параметры, чтобы g++ не ругался
        if (!data || size == 0 || !out_size) return nullptr;

        uint8_t* result = static_cast<uint8_t*>(malloc(size));
        if (!result) return nullptr;

        for (size_t i = 0; i < size; ++i) {
            result[i] = static_cast<uint8_t>(255 - data[i]);
        }

        *out_size = size;
        return result;
    }

    // Дешифрование Атбаш взаимно обратное: инверсия инверсии возвращает исходный байт
    uint8_t* decrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size) {
        return encrypt(data, size, key, key_size, out_size);
    }

    void free_buffer(uint8_t* buffer) {
        free(buffer);
    }
}