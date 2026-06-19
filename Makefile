CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS =

ifeq ($(OS),Windows_NT)
	LIB_PREFIX =
	LIB_EXT = dll
	SHARED_FLAG = -shared
else
	UNAME_S := $(shell uname -s)
	LIB_PREFIX = lib

	ifeq ($(UNAME_S),Darwin)
		LIB_EXT = dylib
		SHARED_FLAG = -dynamiclib -fPIC
	else
		LIB_EXT = so
		SHARED_FLAG = -shared -fPIC
		LDFLAGS = -ldl
	endif
endif

.PHONY: all rc4 chacha20 atbash vernam vigenere elgamal cryptum clean

all: rc4 chacha20 atbash vernam vigenere elgamal cryptum

rc4:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/rc4/rc4.cpp -o algorithms/rc4/$(LIB_PREFIX)rc4.$(LIB_EXT)

chacha20:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/chacha20/chacha20.cpp -o algorithms/chacha20/$(LIB_PREFIX)chacha20.$(LIB_EXT)

atbash:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/atbash/Atbash.cpp -o algorithms/atbash/$(LIB_PREFIX)atbash.$(LIB_EXT)

vernam:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/vernam/Vernam.cpp -o algorithms/vernam/$(LIB_PREFIX)vernam.$(LIB_EXT)

vigenere:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/vigenere/vigenere.cpp -o algorithms/vigenere/$(LIB_PREFIX)vigenere.$(LIB_EXT)

elgamal:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/elgamal/elgamal.cpp -o algorithms/elgamal/$(LIB_PREFIX)elgamal.$(LIB_EXT)

cryptum:
	$(CXX) $(CXXFLAGS) main.cpp -o cryptum $(LDFLAGS)

clean:
	rm -f cryptum cryptum.exe
	rm -f algorithms/rc4/librc4.so algorithms/rc4/librc4.dylib algorithms/rc4/rc4.dll
	rm -f algorithms/chacha20/libchacha20.so algorithms/chacha20/libchacha20.dylib algorithms/chacha20/chacha20.dll
	rm -f algorithms/atbash/libatbash.so algorithms/atbash/libatbash.dylib algorithms/atbash/atbash.dll
	rm -f algorithms/vernam/libvernam.so algorithms/vernam/libvernam.dylib algorithms/vernam/vernam.dll
	rm -f algorithms/vigenere/libvigenere.so algorithms/vigenere/libvigenere.dylib algorithms/vigenere/vigenere.dll
	rm -f algorithms/elgamal/libelgamal.so algorithms/elgamal/libelgamal.dylib algorithms/elgamal/elgamal.dll
	rm -f algorithms/*/*.cpp.*
