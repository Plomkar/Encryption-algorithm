#include "crypto_interface.h"
#include <cstdlib>
#include <ctime>

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        int shift = key.empty() ? 3 : static_cast<int>(key[0]);
        if (!encrypt) shift = -shift;

        for (size_t i = 0; i < size; ++i) {
            out[i] = static_cast<unsigned char>((in[i] + shift + 256) % 256);
        }
    }

    std::string generate_key() {
        srand(static_cast<unsigned int>(time(nullptr)));
        std::string k = "";
        k += static_cast<char>(1 + rand() % 25);
        return k;
    }
}