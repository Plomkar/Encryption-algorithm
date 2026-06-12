#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dlfcn.h>
#include <sys/stat.h>
#include <clocale>
#include <sstream>
#include <iomanip>
#include <cctype>

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

// Переводит бинарную строку (с любыми байтами) в читаемый HEX-вид
std::string to_hex(const std::string& input) {
    std::ostringstream oss;
    for (unsigned char c : input) {
        oss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>(c);
    }
    return oss.str();
}

// Переводит введенную пользователем HEX-строку обратно в бинарный вид
std::string from_hex(const std::string& hex_input) {
    std::string result;
    std::string clean_hex = "";
    
    // Удаляем пробелы, если пользователь случайно их вставил
    for (char c : hex_input) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            clean_hex += c;
        }
    }
    
    if (clean_hex.empty()) {
        return "";
    }
    
    if (clean_hex.length() % 2 != 0) {
        throw std::runtime_error("Длина HEX-строки должна быть чётной!");
    }

    for (size_t i = 0; i < clean_hex.length(); i += 2) {
        std::string byteString = clean_hex.substr(i, 2);
        char byte = static_cast<char>(std::strtol(byteString.c_str(), nullptr, 16));
        result += byte;
    }
    return result;
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
    std::cout << "\nГлавное меню (Выбор шифра)\n";
    std::cout << "1. Цезарь\n";
    std::cout << "2. Атбаш\n";
    std::cout << "3. XOR\n";
    std::cout << "4. Виженер\n";
    std::cout << "5. Хилл\n";
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

        // Внутренний цикл для работы с выбранным шифром
        bool keep_working_with_cipher = true;
        while (keep_working_with_cipher) {
            std::cout << "\nРежим работы с выбранным шифром\n";
            std::cout << "1. Запустить генератор ключей\n";
            std::cout << "2. Работа с текстом\n";
            std::cout << "3. Работа с файлом\n";
            std::cout << "0. Сменить шифр (вернуться в главное меню)\n";
            std::cout << "Выберите режим: ";
            
            int mode;
            if (!(std::cin >> mode)) {
                std::cout << "Некорректный ввод. Возврат в главное меню.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                break;
            }

            if (mode == 0) {
                keep_working_with_cipher = false;
                continue; // Выходим во внешний цикл к выбору шифров
            }

            try {
                if (mode == 1) {
                    std::string generated = run_key_generator(choice);
                    std::cout << "Сгенерированный ключ (HEX): " << to_hex(generated) << "\n";
                } 
                else if (mode == 2) {
                    std::cin.ignore();
                    
                    std::cout << "1. Зашифровать\n2. Расшифровать\nВыбор: ";
                    int op; std::cin >> op;
                    bool enc = (op == 1);
                    std::cin.ignore(); // Очищаем буфер после ввода числа

                    std::cout << "Введите ключ (в формате HEX): ";
                    std::string hex_key;
                    std::getline(std::cin, hex_key);
                    std::string key = from_hex(hex_key);

                    // ПРОВЕРКА: Если ключ пустой и это НЕ шифр Атбаш
                    if (key.empty() && choice != CipherType::Atbash) {
                        throw std::runtime_error("Ключ не может быть пустым для выбранного алгоритма!");
                    }

                    std::vector<unsigned char> in_buf;

                    if (enc) {
                        std::cout << "Введите строку: ";
                        std::string text;
                        std::getline(std::cin, text);
                        
                        // ПРОВЕРКА: Защита от пустой строки при шифровании
                        if (text.empty()) {
                            throw std::runtime_error("Строка для шифрования не может быть пустой!");
                        }
                        
                        in_buf.assign(text.begin(), text.end());
                    } else {
                        std::cout << "Введите зашифрованный текст (в формате HEX): ";
                        std::string hex_text;
                        std::getline(std::cin, hex_text);
                        
                        // ПРОВЕРКА: Защита от пустой строки при расшифровании
                        if (hex_text.empty()) {
                            throw std::runtime_error("Строка с HEX-текстом не может быть пустой!");
                        }
                        
                        std::string bin_text = from_hex(hex_text);
                        in_buf.assign(bin_text.begin(), bin_text.end());
                    }

                    std::vector<unsigned char> out_buf(in_buf.size());

                    run_cipher(choice, in_buf, out_buf, key, enc);

                    if (enc) {
                        std::string raw_res(out_buf.begin(), out_buf.end());
                        std::cout << "Результат (HEX): " << to_hex(raw_res) << "\n";
                    } else {
                        std::string res(out_buf.begin(), out_buf.end());
                        std::cout << "Результат: " << res << "\n";
                    }
                } 
                else if (mode == 3) {
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

                    std::cout << "Введите ключ (в формате HEX): ";
                    std::string hex_key;
                    std::getline(std::cin, hex_key);
                    std::string key = from_hex(hex_key);

                    // ПРОВЕРКА: Для файлов требуем ключ (если это не Атбаш)
                    if (key.empty() && choice != CipherType::Atbash) {
                        throw std::runtime_error("Ключ не может быть пустым для выбранного алгоритма!");
                    }

                    std::cout << "1. Зашифровать\n2. Расшифровать\nВыбор: ";
                    int op; std::cin >> op;
                    bool enc = (op == 1);

                    std::ifstream infile(in_path, std::ios::binary | std::ios::ate);
                    if (!infile.is_open()) throw std::runtime_error("Не удалось открыть файл ввода.");
                    
                    std::streamsize size = infile.tellg();
                    infile.seekg(0, std::ios::beg);

                    // ПРОВЕРКА: Защита от обработки пустого файла (0 байт)
                    if (size == 0) {
                        infile.close();
                        throw std::runtime_error("Выбранный файл пуст! Нечего шифровать.");
                    }

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
                else {
                    std::cout << "Неверный режим работы.\n";
                }
            } 
            catch (const std::exception& e) {
                // Все throw std::runtime_error будут красиво перехватываться здесь
                std::cerr << "Произошел сбой: " << e.what() << "\n";
            }
        }
    }

    std::cout << "Программа завершила работу.\n";
    return 0;
}