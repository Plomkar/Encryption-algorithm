#include "crypto_interface.h"

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        (void)key; (void)encrypt; // Игнорируем неиспользуемые параметры
        for (size_t i = 0; i < size; ++i) {
            out[i] = static_cast<unsigned char>(255 - in[i]);
        }
    }

    std::string generate_key() {
        return "";
    }
}
