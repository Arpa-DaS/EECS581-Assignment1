CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic

all: ipv4_extract

ipv4_extract: main.cpp ipv4.cpp ipv4.h
	$(CXX) $(CXXFLAGS) -o $@ main.cpp ipv4.cpp

run_tests: tests.cpp ipv4.cpp ipv4.h
	$(CXX) $(CXXFLAGS) -o $@ tests.cpp ipv4.cpp

test: run_tests
	./run_tests

clean:
	rm -f ipv4_extract run_tests

.PHONY: all test clean
