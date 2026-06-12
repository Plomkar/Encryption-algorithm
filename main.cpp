#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dlfcn.h>
#include <sys/stat.h>
#include <clocale>

// Прототипы функций из интерфейса библиотек
typedef void (*process_data_t)(const unsigned char*, size_t, unsigned char*, const std::string&, bool);
typedef std::string (*generate_key_t)();

// Перечисление типов шифров с помощью enum class
enum class CipherType {
    Exit = 0,
    Caesar = 1,
    Atbash = 2,
    Xor = 3,
    Vigenere = 4,
    Hill = 5,
    Aes = 6,
    Unknown
};

// Проверка существования файла
bool file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Преобразование пути .so файла на основе перечисления
std::string get_lib_name(CipherType type) {
    switch (type) {
        case CipherType::Caesar:   return "./libcaesar.so";
        case CipherType::Atbash:   return "./libatbash.so";
        case CipherType::Xor:      return "./libxor.so";
        case CipherType::Vigenere: return "./libvigenere.so";
        case CipherType::Hill:     return "./libhill.so";
        case CipherType::Aes:      return "./libaes.so";
        default:                   return "";
    }
}

void run_cipher(CipherType type, const std::vector<unsigned char>& input, 
                std::vector<unsigned char>& output, const std::string& key, bool encrypt) {
    
    std::string lib_name = get_lib_name(type);
    if (lib_name.empty()) throw std::runtime_error("Неверный тип шифра.");

    void* handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("Не удалось загрузить библиотеку: " + std::string(dlerror()));
    }

    process_data_t proc = (process_data_t)dlsym(handle, "process_data");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        dlclose(handle);
        throw std::runtime_error("Ошибка поиска функции: " + std::string(dlsym_error));
    }

    proc(input.data(), input.size(), output.data(), key, encrypt);
    dlclose(handle);
}

std::string run_key_generator(CipherType type) {
    std::string lib_name = get_lib_name(type);
    if (lib_name.empty()) throw std::runtime_error("Неверный тип шифра.");

    void* handle = dlopen(lib_name.c_str(), RTLD_LAZY);
    if (!handle) throw std::runtime_error("Ошибка загрузки библиотеки.");

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
    std::cout << "\nEncryption Algorithm RGR\n";
    std::cout << "1. Цезарь\n";
    std::cout << "2. Атбаш\n";
    std::cout << "3. XOR\n";
    std::cout << "4. Виженер\n";
    std::cout << "5. Хилл (libhill.so)\n";
    std::cout << "6. AES\n";
    std::cout << "0. Выход из программы\n";
    std::cout << "Выберите алгоритм: ";
}

int main() {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");

    while (true) {
        show_menu();
        int int_choice;
        if (!(std::cin >> int_choice)) {
            std::cout << "Некорректный ввод. Завершение.\n";
            break;
        }

        // Приведение введенного int к enum class
        CipherType choice = CipherType::Unknown;
        if (int_choice >= 0 && int_choice <= 6) {
            choice = static_cast<CipherType>(int_choice);
        }

        if (choice == CipherType::Exit) break;
        if (choice == CipherType::Unknown) {
            std::cout << "Неверный пункт меню.\n";
            continue;
        }

        std::cout << "\n1. Запустить генератор ключей\n2. Работа с текстом\n3. Работа с файлом\n0. Вернуться назад\nВыберите режим: ";
        int mode;
        std::cin >> mode;

        if (mode == 0) {
            std::cout << "Возврат в главное меню.\n";
            continue; // Возвращает пользователя к выбору шифра
        }

        try {
            if (mode == 1) {
                std::string generated = run_key_generator(choice);
                std::cout << "Сгенерированный ключ: " << generated << "\n";
                
                // Сохранение ключа в отдельный файл key.key с перезаписью
                std::ofstream key_file("key.key", std::ios::trunc); 
                if (key_file.is_open()) {
                    key_file << generated;
                    key_file.close();
                    std::cout << "Ключ успешно сохранен и обновлен в файле: key.key\n";
                } else {
                    std::cout << "Предупреждение: Не удалось открыть файл key.key для записи.\n";
                }
            } 
            else if (mode == 2) {
                std::cout << "\n1. Зашифровать\n2. Расшифровать\n0. Вернуться назад\nВыбор: ";
                int op; std::cin >> op;
                if (op == 0) continue;
                bool enc = (op == 1);

                std::cin.ignore();
                std::cout << "Введите строку: ";
                std::string text;
                std::getline(std::cin, text);

                std::cout << "Введите ключ: ";
                std::string key;
                std::getline(std::cin, key);

                std::vector<unsigned char> in_buf(text.begin(), text.end());
                std::vector<unsigned char> out_buf(in_buf.size());

                run_cipher(choice, in_buf, out_buf, key, enc);

                std::string res(out_buf.begin(), out_buf.end());
                std::cout << "Результат: " << res << "\n";
            } 
            else if (mode == 3) {
                std::cout << "\n1. Зашифровать\n2. Расшифровать\n0. Вернуться назад\nВыбор: ";
                int op; std::cin >> op;
                if (op == 0) continue;
                bool enc = (op == 1);

                std::cin.ignore();
                std::cout << "Введите путь к исходному файлу: ";
                std::string in_path;
                std::getline(std::cin, in_path);

                if (!file_exists(in_path)) {
                    std::cout << "Ошибка: Файл не найден!\n";
                    continue;
                }

                std::cout << "Введите путь к результирующему файлу: ";
                std::string out_path;
                std::getline(std::cin, out_path);

                std::cout << "Введите ключ: ";
                std::string key;
                std::getline(std::cin, key);

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

                run_cipher(choice, in_buf, out_buf, key, enc);

                std::ofstream outfile(out_path, std::ios::binary);
                if (!outfile.is_open()) throw std::runtime_error("Не удалось открыть файл вывода.");
                outfile.write(reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
                outfile.close();

                std::cout << "Операция успешно завершена. Файл сохранен по пути: " << out_path << "\n";
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "Произошел сбой: " << e.what() << "\n";
        }
    }

    std::cout << "Программа завершила работу.\n";
    return 0;
}