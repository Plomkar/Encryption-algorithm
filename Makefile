CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic -fPIC

all: libs main

libs:
	$(CXX) $(CXXFLAGS) -shared caesar.cpp -o libcaesar.so
	$(CXX) $(CXXFLAGS) -shared atbash.cpp -o libatbash.so

main: main.cpp
	$(CXX) -Wall -Wextra -std=c++17 main.cpp -o rgr_app -ldl

clean:
	rm -f rgr_app *.so