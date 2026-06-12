#ifndef CRYPTO_INTERFACE_H
#define CRYPTO_INTERFACE_H

#include <string>

extern "C" {
    // Единый интерфейс обработки данных
    void process_data(const unsigned char* in_data, size_t data_size, 
                      unsigned char* out_data, const std::string& key, 
                      bool encrypt);
                      
    // Единый интерфейс генерации ключа
    std::string generate_key();
}

#endif