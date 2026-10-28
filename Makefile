CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic

.PHONY: all test clean
all: gf_multiply

gf_multiply: main.cpp gf2n.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o gf_multiply

test_gf2n: tests/test_gf2n.cpp gf2n.hpp
	$(CXX) $(CXXFLAGS) tests/test_gf2n.cpp -o test_gf2n

test: test_gf2n
	./test_gf2n

clean:
	rm -f gf_multiply test_gf2n gf_multiply.exe test_gf2n.exe
