CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Директории
ATBASH_DIR = algorithms/atbash
BIN_DIR = bin

all: atbash_lib cryptum

# Создание директории bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Сборка библиотеки Atbash (динамическая)
atbash_lib: $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -dynamiclib $(ATBASH_DIR)/atbash.cpp -o $(BIN_DIR)/libatbash.dylib

# Сборка библиотеки Atbash (статическая)
atbash_static: $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(ATBASH_DIR)/atbash.cpp -o $(BIN_DIR)/atbash.o
	ar rcs $(BIN_DIR)/libatbash.a $(BIN_DIR)/atbash.o

# Сборка основной программы с использованием библиотеки
cryptum: $(BIN_DIR)
	$(CXX) $(CXXFLAGS) main.cpp -o $(BIN_DIR)/cryptum

# Сборка основной программы с линковкой динамической библиотеки
cryptum_linked: $(BIN_DIR) atbash_lib
	$(CXX) $(CXXFLAGS) main.cpp -L$(BIN_DIR) -latbash -o $(BIN_DIR)/cryptum

# Запуск программы
run: cryptum
	./$(BIN_DIR)/cryptum

# Очистка
clean:
	rm -rf $(BIN_DIR)
	rm -f $(ATBASH_DIR)/*.o
