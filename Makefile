CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -fPIC
LDFLAGS = -ldl

BIN_DIR = bin
INCLUDE_DIR = include
ALGO_DIR = algorithms
SRC_DIR = src

# Ваши алгоритмы
CIPHERS = atbash vernam

# Определение платформы
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    SO_EXT = so
    LIB_PREFIX = lib
endif

# Цель по умолчанию - компилирует оба шифра
all: $(BIN_DIR) $(CIPHERS) cryptum

# Создание директорий
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/tests
	@for cipher in $(CIPHERS); do \
	    mkdir -p $(BIN_DIR)/$$cipher; \
	done

# Компиляция библиотек
$(CIPHERS): %: $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -shared $(ALGO_DIR)/$*/$*.cpp -I$(INCLUDE_DIR) -o $(BIN_DIR)/$*/$(LIB_PREFIX)$*.$(SO_EXT)
	@echo "✅ $* library compiled"

# Компиляция обоих шифров одновременно
both: $(CIPHERS)
	@echo "✅ Both Atbash and Vernam libraries compiled successfully"

# Сборка главной программы
cryptum: $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/main.cpp -I$(INCLUDE_DIR) $(LDFLAGS) -o $(BIN_DIR)/cryptum
	@echo "✅ Cryptum executable compiled"

# Тесты
tests: $(BIN_DIR) both
	$(CXX) $(CXXFLAGS) tests/test_atbash_vernam.cpp -I$(INCLUDE_DIR) $(LDFLAGS) -o $(BIN_DIR)/tests/test_atbash_vernam
	@echo "✅ Tests compiled"

# Запуск тестов
test: tests
	./$(BIN_DIR)/tests/test_atbash_vernam

# Очистка
clean:
	rm -rf $(BIN_DIR)
	@echo "🧹 Cleaned"

# Пересборка
rebuild: clean all

# Запуск программы
run: cryptum
	./$(BIN_DIR)/cryptum --help

# Показать информацию
info:
	@echo "Platform: $(UNAME_S)"
	@echo "Ciphers: $(CIPHERS)"
	@echo "Library prefix: $(LIB_PREFIX)"
	@echo "Library extension: $(SO_EXT)"

.PHONY: all clean run test tests rebuild info both $(CIPHERS) cryptum