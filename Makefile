CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

ifeq ($(OS),Windows_NT)
    LIB_EXT = dll
    SHARED_FLAG = -shared
else
    UNAME_S := $(shell uname -s)

    ifeq ($(UNAME_S),Darwin)
        LIB_EXT = dylib
        SHARED_FLAG = -dynamiclib
    else
        LIB_EXT = so
        SHARED_FLAG = -shared
    endif
endif

all: rc4 chacha20 cryptum

rc4:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/rc4/rc4.cpp -o algorithms/rc4/librc4.$(LIB_EXT)

chacha20:
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) algorithms/chacha20/chacha20.cpp -o algorithms/chacha20/libchacha20.$(LIB_EXT)

cryptum:
	$(CXX) $(CXXFLAGS) main.cpp -o cryptum

clean:
	rm -f cryptum
	rm -f algorithms/rc4/librc4.*
	rm -f algorithms/chacha20/libchacha20.*