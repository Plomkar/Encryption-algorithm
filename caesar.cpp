#include "crypto_interface.h"
#include <cstdlib>

extern "C" {
    // Шифрование Цезаря: (byte + key) % 256
    uint8_t* encrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size) {
        if (!data || size == 0 || !key || key_size == 0 || !out_size) return nullptr;

        uint8_t* result = static_cast<uint8_t*>(malloc(size));
        if (!result) return nullptr;

        // В качестве сдвига берем первый байт ключа
        uint8_t shift = key[0]; 

        for (size_t i = 0; i < size; ++i) {
            result[i] = static_cast<uint8_t>((data[i] + shift) % 256);
        }

        *out_size = size;
        return result;
    }

    // Дешифрование Цезаря: (byte - key + 256) % 256
    uint8_t* decrypt(const uint8_t* data, size_t size, const uint8_t* key, size_t key_size, size_t* out_size) {
        if (!data || size == 0 || !key || key_size == 0 || !out_size) return nullptr;

        uint8_t* result = static_cast<uint8_t*>(malloc(size));
        if (!result) return nullptr;

        uint8_t shift = key[0];

        for (size_t i = 0; i < size; ++i) {
            result[i] = static_cast<uint8_t>((data[i] - shift + 256) % 256);
        }

        *out_size = size;
        return result;
    }

    void free_buffer(uint8_t* buffer) {
        free(buffer);
    }
}