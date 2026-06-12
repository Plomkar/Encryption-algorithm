#include "crypto_interface.h"
#include <cstdlib>
#include <ctime>

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        std::string real_key = key.empty() ? "StandardAESKey12" : key;
        while (real_key.length() < 16) real_key += "x";

        for (size_t i = 0; i < size; ++i) {
            unsigned char byte = in[i];
            if (encrypt) {
                byte ^= real_key[i % 16];
                byte = static_cast<unsigned char>((byte << 3) | (byte >> 5)); 
            } else {
                byte = static_cast<unsigned char>((byte >> 3) | (byte << 5));
                byte ^= real_key[i % 16];
            }
            out[i] = byte;
        }
    }

    std::string generate_key() {
        srand(static_cast<unsigned int>(time(nullptr)));
        std::string k = "";
        for(int i = 0; i < 16; ++i) k += static_cast<char>(rand() % 256);
        return k;
    }
}