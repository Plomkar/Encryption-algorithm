#include "crypto_interface.h"

extern "C" {
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        int a = 3, b = 3, c = 2, d = 5; 
        if (key.length() >= 4) {
            a = key[0]; b = key[1]; c = key[2]; d = key[3];
        }

        if (!encrypt) {
            int det = (a * d - b * c) % 256;
            if (det < 0) det += 256;
            
            int inv_det = 1;
            for (int i = 1; i < 256; i += 2) {
                if ((det * i) % 256 == 1) {
                    inv_det = i;
                    break;
                }
            }
            int adj_a = d, adj_b = -b, adj_c = -c, adj_d = a;
            a = (adj_a * inv_det) % 256;
            b = (adj_b * inv_det) % 256;
            c = (adj_c * inv_det) % 256;
            d = (adj_d * inv_det) % 256;
            if (a < 0) { a += 256; }
            if (b < 0) { b += 256; }
            if (c < 0) { c += 256; }
            if (d < 0) { d += 256; }
        }

        for (size_t i = 0; i < size; i += 2) {
            if (i + 1 < size) {
                out[i] = static_cast<unsigned char>((a * in[i] + b * in[i+1]) % 256);
                out[i+1] = static_cast<unsigned char>((c * in[i] + d * in[i+1]) % 256);
            } else {
                out[i] = in[i];
            }
        }
    }

    std::string generate_key() {
        // Ключ-матрица по умолчанию
        return "\x03\x03\x02\x05"; 
    }
}