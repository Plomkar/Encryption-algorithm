#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dlfcn.h>
#include <sys/stat.h>

// Прототипы функций из интерфейса библиотек
typedef void (*process_data_t)(const unsigned char*, size_t, unsigned char*, const std::string&, bool);
typedef std::string (*generate_key_t)();

// Функция проверки существования файла (ГОСТ 56939-2024 Валидация)
bool file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Динамический вызов библиотеки
void run_cipher(const std::string& lib_name, const std::vector<unsigned char>& input, 
                std::vector<unsigned char>& output, const std::string& key, bool encrypt) {
    
    // Загрузка динамической библиотеки (.so)
    void* handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("Не удалось загрузить библиотеку: " + std::string(dlerror()));
    }

    // Поиск функции обработки данных
    process_data_t proc = (process_data_t)dlsym(handle, "process_data");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        dlclose(handle);
        throw std::runtime_error("Ошибка поиска функции: " + std::string(dlsym_error));
    }

    // Выполнение операции
    proc(input.data(), input.size(), output.data(), key, encrypt);

    // Выгрузка библиотеки из памяти
    dlclose(handle);
}

// Динамический вызов генератора ключей
std::string run_key_generator(const std::string& lib_name) {
    void* handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!handle) throw std::runtime_error("Ошибка загрузки библиотеки для генерации ключа.");

    generate_key_t gen = (generate_key_t)dlsym(handle, "generate_key");
    if (!gen) {
        dlclose(handle);
        throw std::runtime_error("Ошибка поиска функции генератора.");
    }

    std::string key = gen();
    dlclose(handle);
    return key;
}

void show_menu() {
    std::cout << "\n=== Encryption Algorithm RGR ===\n";
    std::cout << "1. Цезарь (libcaesar.so)\n";
    std::cout << "2. Атбаш (libatbash.so)\n";
    std::cout << "3. XOR (libxor.so)\n";
    std::cout << "4. Виженер (libvigenere.so)\n";
    std::cout << "5. Хилл (libhill.so)\n";
    std::cout << "6. AES (libaes.so)\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите алгоритм: ";
}

int main() {
    // Настройка локализации под русский язык
    std::setlocale(LC_ALL, "ru_RU.UTF-8");

    while (true) {
        show_menu();
        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Некорректный ввод. Завершение.\n";
            break;
        }

        if (choice == 0) break;

        std::string lib_name = "";
        switch (choice) {
            case 1: lib_name = "./libcaesar.so"; break;
            case 2: lib_name = "./libatbash.so"; break;
            case 3: lib_name = "./libxor.so"; break;
            case 4: lib_name = "./libvigenere.so"; break;
            case 5: lib_name = "./libhill.so"; break;
            case 6: lib_name = "./libaes.so"; break;
            default: std::cout << "Неверный пункт меню.\n"; continue;
        }

        std::cout << "\n1. Запустить генератор ключей\n2. Шифровать/Дешифровать текст\n3. Шифровать/Дешифровать файл\nВыберите режим: ";
        int mode;
        std::cin >> mode;

        try {
            if (mode == 1) {
                std::string generated = run_key_generator(lib_name);
                std::cout << "Сгенерированный ключ: " << generated << " (в байтовом/строковом виде)\n";
            } 
            else if (mode == 2) {
                std::cin.ignore();
                std::cout << "Введите строку: ";
                std::string text;
                std::getline(std::cin, text);

                std::cout << "Введите ключ: ";
                std::string key;
                std::getline(std::cin, key);

                std::cout << "1. Зашифровать\n2. Расшифровать\nВыбор: ";
                int op; std::cin >> op;
                bool enc = (op == 1);

                std::vector<unsigned char> in_buf(text.begin(), text.end());
                std::vector<unsigned char> out_buf(in_buf.size());

                run_cipher(lib_name, in_buf, out_buf, key, enc);

                std::string res(out_buf.begin(), out_buf.end());
                std::cout << "Результат: " << res << "\n";
            } 
            else if (mode == 3) {
                std::cin.ignore();
                std::cout << "Введите путь к исходному файлу: ";
                std::string in_path;
                std::getline(std::cin, in_path);

                if (!file_exists(in_path)) {
                    std::cout << "Ошибка: Файл не найден! [ГОСТ 56939-2024]\n";
                    continue;
                }

                std::cout << "Введите путь к результирующему файлу: ";
                std::string out_path;
                std::getline(std::cin, out_path);

                std::cout << "Введите ключ: ";
                std::string key;
                std::getline(std::cin, key);

                std::cout << "1. Зашифровать\n2. Расшифровать\nВыбор: ";
                int op; std::cin >> op;
                bool enc = (op == 1);

                // Чтение бинарного файла
                std::ifstream infile(in_path, std::ios::binary | std::ios::ate);
                if (!infile.is_open()) throw std::runtime_error("Не удалось открыть файл ввода.");
                
                std::streamsize size = infile.tellg();
                infile.seekg(0, std::ios::beg);

                std::vector<unsigned char> in_buf(size);
                if (!infile.read(reinterpret_cast<char*>(in_buf.data()), size)) {
                    throw std::runtime_error("Ошибка чтения данных из файла.");
                }
                infile.close();

                std::vector<unsigned char> out_buf(size);

                // Вызов алгоритма из библиотеки
                run_cipher(lib_name, in_buf, out_buf, key, enc);

                // Запись в бинарный файл
                std::ofstream outfile(out_path, std::ios::binary);
                if (!outfile.is_open()) throw std::runtime_error("Не удалось открыть файл вывода.");
                outfile.write(reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
                outfile.close();

                std::cout << "Операция успешно завершена. Файл сохранен по пути: " << out_path << "\n";
            }
        } 
        catch (const std::exception& e) {
            // Обработка исключений согласно критерию надежности ТЗ (try-catch)
            std::cerr << "Произошел сбой: " << e.what() << "\n";
        }
    }

    std::cout << "Программа завершила работу.\n";
    return 0;
}