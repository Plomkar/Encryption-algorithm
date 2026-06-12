#include "crypto_interface.h"
#include <string>
#include <vector>
#include <random>

//для безопасного взятия остатка по модулю 256.
// всегда возвращает число от 0 до 255,
inline int mod256(int value) {
    int res = value % 256;
    if (res < 0) res += 256;
    return res;
}

extern "C" {
    
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        // Коэффициенты матрицы по умолчанию
        int a = 3, b = 3, c = 2, d = 5; 
        
        if (key.length() >= 4) {
            a = static_cast<unsigned char>(key[0]); 
            b = static_cast<unsigned char>(key[1]); 
            c = static_cast<unsigned char>(key[2]); 
            d = static_cast<unsigned char>(key[3]);
        }

        if (!encrypt) {
            int det = mod256(a * d - b * c);
            
            if (det % 2 == 0) {
                for (size_t i = 0; i < size; ++i) out[i] = 0;
                return; 
            }
            
            int inv_det = 1;
            for (int i = 1; i < 256; i += 2) {
                if (mod256(det * i) == 1) {
                    inv_det = i;
                    break;
                }
            }

            int adj_a = d;
            int adj_b = -b;
            int adj_c = -c;
            int adj_d = a;

            // компоненты обратной матрицы: K^-1 = inv_det * adj(K)
            a = mod256(adj_a * inv_det);
            b = mod256(adj_b * inv_det);
            c = mod256(adj_c * inv_det);
            d = mod256(adj_d * inv_det);
        }

        for (size_t i = 0; i < size; i += 2) {
            if (i + 1 < size) {
                // Умножаем матрицу ключа на вектор открытого/шифрованного текста
                out[i]   = static_cast<unsigned char>(mod256(a * in[i] + b * in[i+1]));
                out[i+1] = static_cast<unsigned char>(mod256(c * in[i] + d * in[i+1]));
            } else {
                out[i] = in[i];
            }
        }
    }

    std::string generate_key() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 255);

        int a, b, c, d, det;

        do {
            a = dist(gen);
            b = dist(gen);
            c = dist(gen);
            d = dist(gen);
            
            det = mod256(a * d - b * c);
        } while (det % 2 == 0);

        std::string key = "";
        key += static_cast<char>(a);
        key += static_cast<char>(b);
        key += static_cast<char>(c);
        key += static_cast<char>(d);

        return key;
    }
}