#ifndef CRYPTO_INTERFACE_H
#define CRYPTO_INTERFACE_H

#include <string>

extern "C" {
    void process_data(const unsigned char* in_data, size_t data_size, 
                      unsigned char* out_data, const std::string& key, 
                      bool encrypt);
                      
    std::string generate_key();
}

#endif