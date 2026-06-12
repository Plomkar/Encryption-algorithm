#include "crypto_interface.h"
#include <string>
#include <vector>
#include <random>

// Вспомогательная функция для безопасного взятия остатка по модулю 256.
// В отличие от стандартного оператора % в C++, она всегда возвращает число от 0 до 255,
// даже если на вход пришло глубоко отрицательное число.
inline int mod256(int value) {
    int res = value % 256;
    if (res < 0) res += 256;
    return res;
}

extern "C" {
    
    void process_data(const unsigned char* in, size_t size, unsigned char* out, const std::string& key, bool encrypt) {
        // Коэффициенты матрицы по умолчанию: det = 3*5 - 3*2 = 9 (нечетный, отличный ключ)
        int a = 3, b = 3, c = 2, d = 5; 
        
        // Если пользователь передал свой ключ (минимум 4 байта), парсим его
        if (key.length() >= 4) {
            a = static_cast<unsigned char>(key[0]); 
            b = static_cast<unsigned char>(key[1]); 
            c = static_cast<unsigned char>(key[2]); 
            d = static_cast<unsigned char>(key[3]);
        }

        if (!encrypt) {
            // Вычисляем детерминант матрицы 2x2
            int det = mod256(a * d - b * c);
            
            // В кольце вычетов по модулю 256 обратная матрица существует
            // тогда и только тогда, когда детерминант НЕЧЕТНЫЙ (взаимно прост с 256).
            if (det % 2 == 0) {
                // Если ключ математически непригоден для расшифрования,
                // забиваем выход нулями во избежание утечки мусора и выходим.
                for (size_t i = 0; i < size; ++i) out[i] = 0;
                return; 
            }
            
            // Поиск мультипликативного обратного для детерминанта: (det * inv_det) % 256 == 1
            int inv_det = 1;
            for (int i = 1; i < 256; i += 2) {
                if (mod256(det * i) == 1) {
                    inv_det = i;
                    break;
                }
            }

            // Находим компоненты союзной (адъюнктивной) матрицы
            int adj_a = d;
            int adj_b = -b;
            int adj_c = -c;
            int adj_d = a;

            // Вычисляем компоненты обратной матрицы: K^-1 = inv_det * adj(K)
            a = mod256(adj_a * inv_det);
            b = mod256(adj_b * inv_det);
            c = mod256(adj_c * inv_det);
            d = mod256(adj_d * inv_det);
        }

        // Построчная обработка данных блоками по 2 байта (векторы размера 2).
        // Благодаря выравниванию в main.cpp, size здесь всегда четный.
        for (size_t i = 0; i < size; i += 2) {
            if (i + 1 < size) {
                // Умножаем матрицу ключа на вектор открытого/шифрованного текста
                out[i]   = static_cast<unsigned char>(mod256(a * in[i] + b * in[i+1]));
                out[i+1] = static_cast<unsigned char>(mod256(c * in[i] + d * in[i+1]));
            } else {
                // Защитный fallback на случай, если что-то пошло не так с выравниванием
                out[i] = in[i];
            }
        }
    }

    std::string generate_key() {
        // Используем современный и надежный генератор случайных чисел
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 255);

        int a, b, c, d, det;

        // Генерируем случайную матрицу до тех пор, пока её детерминант не станет нечетным
        do {
            a = dist(gen);
            b = dist(gen);
            c = dist(gen);
            d = dist(gen);
            
            det = mod256(a * d - b * c);
        } while (det % 2 == 0); // Перегенерировать, если детерминант четный или равен 0

        // Собираем валидную случайную матрицу в 4-байтную строку
        std::string key = "";
        key += static_cast<char>(a);
        key += static_cast<char>(b);
        key += static_cast<char>(c);
        key += static_cast<char>(d);

        return key;
    }
}