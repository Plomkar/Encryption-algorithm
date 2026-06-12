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
        for(int i = 0; i < 16; ++i) {
            int type = rand() % 3;
            if (type == 0) {
                k += static_cast<char>('0' + (rand() % 10)); // Цифры (ASCII 48-57)
            } else if (type == 1) {
                k += static_cast<char>('A' + (rand() % 26)); // Заглавные (ASCII 65-90)
            } else {
                k += static_cast<char>('a' + (rand() % 26)); // Строчные (ASCII 97-122)
            }
        }
        return k;
    }
}