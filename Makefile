CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS =

ifeq ($(OS),Windows_NT)
	LIB_EXT = dll
	SHARED_FLAG = -shared
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Darwin)
		LIB_EXT = dylib
		SHARED_FLAG = -dynamiclib -fPIC
	else
		LIB_EXT = so
		SHARED_FLAG = -shared -fPIC
		LDFLAGS = -ldl
	endif
endif

.PHONY: all vigenere elgamal cryptum clean

all: vigenere elgamal cryptum

vigenere:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/vigenere/vigenere.cpp -o algorithms/vigenere/libvigenere.$(LIB_EXT)

elgamal:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/elgamal/elgamal.cpp -o algorithms/elgamal/libelgamal.$(LIB_EXT)

cryptum:
	$(CXX) $(CXXFLAGS) main.cpp -o cryptum $(LDFLAGS)

clean:
	rm -f cryptum
	rm -f algorithms/vigenere/libvigenere.so
	rm -f algorithms/vigenere/libvigenere.dylib
	rm -f algorithms/vigenere/libvigenere.dll
	rm -f algorithms/elgamal/libelgamal.so
	rm -f algorithms/elgamal/libelgamal.dylib
	rm -f algorithms/elgamal/libelgamal.dll
	rm -f algorithms/vigenere/libvigenere.*-vigenere.cpp.*
	rm -f algorithms/elgamal/libelgamal.*-elgamal.cpp.*