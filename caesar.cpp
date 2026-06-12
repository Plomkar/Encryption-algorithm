#include "crypto_interface.h"
#include <cstdlib>
#include <ctime>

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        // Если ключ пустой — сдвиг 3, иначе берем ASCII-код первого символа как величину сдвига
        int shift = key.empty() ? 3 : static_cast<int>(key[0] % 256);
        if (!encrypt) shift = -shift;

        for (size_t i = 0; i < size; ++i) {
            out[i] = static_cast<unsigned char>((in[i] + shift + 256) % 256);
        }
    }

    std::string generate_key() {
        srand(static_cast<unsigned int>(time(nullptr)));
        std::string k = "";
        // Генерируем случайную заглавную букву от 'B' до 'Z', её код станет величиной сдвига
        k += static_cast<char>('B' + rand() % 25); 
        return k;
    }
}