#include "crypto_interface.h"
#include <cstdlib>
#include <ctime>

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        (void)encrypt;
        if (key.empty()) {
            for (size_t i = 0; i < size; ++i) out[i] = in[i];
            return;
        }
        for (size_t i = 0; i < size; ++i) {
            out[i] = in[i] ^ key[i % key.length()];
        }
    }

    std::string generate_key() {
        srand(static_cast<unsigned int>(time(nullptr)));
        std::string k = "";
        for(int i = 0; i < 8; ++i) k += ('A' + rand() % 26);
        return k;
    }
}