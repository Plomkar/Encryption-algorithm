CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fPIC

LIBS = libcaesar.so libatbash.so libxor.so libvigenere.so libhill.so libaes.so
TARGET = main

all: $(LIBS) $(TARGET)

libcaesar.so: caesar.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared caesar.cpp -o libcaesar.so

libatbash.so: atbash.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared atbash.cpp -o libatbash.so

libxor.so: xor.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared xor.cpp -o libxor.so

libvigenere.so: vigenere.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared vigenere.cpp -o libvigenere.so

libhill.so: hill.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared hill.cpp -o libhill.so

libaes.so: aes.cpp crypto_interface.h
	$(CXX) $(CXXFLAGS) -shared aes.cpp -o libaes.so

$(TARGET): main.cpp
	$(CXX) -std=c++17 main.cpp -o $(TARGET) -ldl

clean:
	rm -f *.so $(TARGET)