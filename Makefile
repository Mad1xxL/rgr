CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: rc4 chacha20 cryptum

rc4:
	$(CXX) $(CXXFLAGS) -dynamiclib algorithms/rc4/rc4.cpp -o algorithms/rc4/librc4.dylib

chacha20:
	$(CXX) $(CXXFLAGS) -dynamiclib algorithms/chacha20/chacha20.cpp -o algorithms/chacha20/libchacha20.dylib

cryptum:
	$(CXX) $(CXXFLAGS) main.cpp -o cryptum

clean:
	rm -f cryptum
	rm -f algorithms/rc4/librc4.dylib
	rm -f algorithms/chacha20/libchacha20.dylib