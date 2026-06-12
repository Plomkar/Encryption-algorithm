#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <dlfcn.h> // Заголовочный файл для динамической загрузки в Linux

// Сигнатуры функций из интерфейса
typedef uint8_t* (*crypto_func)(const uint8_t*, size_t, const uint8_t*, size_t, size_t*);
typedef void (*free_func)(uint8_t*);

// Структура для хранения информации о доступных алгоритмах
struct CipherAlgorithm {
    std::string name;
    std::string lib_path;
};

void show_menu() {
    std::cout << "\n=== Encryption Algorithm RGR ===" << std::endl;
    std::cout << "1. Выбор процесса шифрования/дешифрования текста" << std::endl;
    std::cout << "2. Выбор процесса шифрования/дешифрования файла" << std::endl;
    std::cout << "3. Перейти к генератору ключей" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "Выберите действие: ";
}

int select_cipher(const std::vector<CipherAlgorithm>& ciphers) {
    std::cout << "\nДоступные алгоритмы шифрования:" << std::endl;
    for (size_t i = 0; i < ciphers.size(); ++i) {
        std::cout << i + 1 << ". " << ciphers[i].name << std::endl;
    }
    std::cout << "Выберите алгоритм: ";
    int choice;
    std::cin >> choice;
    if (choice < 1 || choice > static_cast<int>(ciphers.size())) {
        throw std::runtime_error("Некорректный выбор алгоритма!");
    }
    return choice - 1;
}

int main() {
    // Список наших будущих `.so` библиотек
    std::vector<CipherAlgorithm> ciphers = {
        {"Цезарь (Caesar)", "./libcaesar.so"},
        {"Атбаш (Atbash)", "./libatbash.so"},
        {"Хилл (Hill)", "./libhill.so"},
        {"AES", "./libaes.so"},
        {"XOR", "./libxor.so"},
        {"Виженер (Vigenere)", "./libvigenere.so"}
    };

    int main_choice = -1;

    while (main_choice != 0) {
        try {
            show_menu();
            if (!(std::cin >> main_choice)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                throw std::runtime_error("Ошибка ввода! Введите число.");
            }

            if (main_choice == 0) break;

            switch (main_choice) {
                case 1: { // Работа с текстом
                    int cipher_idx = select_cipher(ciphers);
                    std::cout << "Выбран алгоритм: " << ciphers[cipher_idx].name << " для обработки ТЕКСТА." << std::endl;
                    // TODO: Здесь будет вызов dlopen() и передача текстового потока
                    break;
                }
                case 2: { // Работа с файлами
                    int cipher_idx = select_cipher(ciphers);
                    std::cout << "Выбран алгоритм: " << ciphers[cipher_idx].name << " для обработки ФАЙЛА." << std::endl;
                    // TODO: Здесь будет логика проверки путей файловых потоков (std::ifstream/std::ofstream)
                    break;
                }
                case 3:
                    std::cout << "[Генератор ключей] Функция в разработке согласно этапам ЖЦ..." << std::endl;
                    break;
                default:
                    std::cout << "Неверный пункт меню. Попробуйте снова." << std::endl;
                    break;
            }
        } 
        catch (const std::exception& e) {
            // Отказоустойчивость: ловим ошибки, не даем программе упасть в SegFault
            std::cerr << "\n[Ошибка]: " << e.what() << std::endl;
        }
    }

    std::cout << "Программа завершила работу." << std::endl;
    return 0;
}