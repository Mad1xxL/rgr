CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS =

ifeq ($(OS),Windows_NT)
LIB_EXT = dll
LIB_PREFIX =
SHARED_FLAG = -shared
PROGRAM = cryptum.exe
else
UNAME_S := $(shell uname -s)

LIB_PREFIX = lib
PROGRAM = cryptum

ifeq ($(UNAME_S),Darwin)
LIB_EXT = dylib
SHARED_FLAG = -dynamiclib -fPIC
else
LIB_EXT = so
SHARED_FLAG = -shared -fPIC
LDFLAGS = -ldl
endif
endif

ATBASH_SRC = algorithms/atbash/Atbash.cpp
VERNAM_SRC = algorithms/vernam/Vernam.cpp

ATBASH_LIB = algorithms/atbash/$(LIB_PREFIX)atbash.$(LIB_EXT)
VERNAM_LIB = algorithms/vernam/$(LIB_PREFIX)vernam.$(LIB_EXT)

.PHONY: all atbash vernam cryptum clean

all: atbash vernam cryptum

atbash: $(ATBASH_LIB)

$(ATBASH_LIB): $(ATBASH_SRC)
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) $(ATBASH_SRC) -o $(ATBASH_LIB)

vernam: $(VERNAM_LIB)

$(VERNAM_LIB): $(VERNAM_SRC)
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) $(VERNAM_SRC) -o $(VERNAM_LIB)

cryptum: main.cpp include/crypto_api.h
	$(CXX) $(CXXFLAGS) main.cpp -o $(PROGRAM) $(LDFLAGS)

clean:
	rm -f cryptum cryptum.exe
	rm -f algorithms/atbash/libatbash.so
	rm -f algorithms/atbash/libatbash.dylib
	rm -f algorithms/atbash/atbash.dll
	rm -f algorithms/vernam/libvernam.so
	rm -f algorithms/vernam/libvernam.dylib
	rm -f algorithms/vernam/vernam.dll