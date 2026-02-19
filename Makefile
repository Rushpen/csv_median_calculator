.PHONY: all fast run clean valgrind install-deps install-boost install-spdlog install-toml install-cmake

# Количество ядер процессора для сборки
CORES := $(shell nproc 2>/dev/null || echo 4)

all: fast

install-cmake:
	@if ! command -v cmake >/dev/null 2>&1; then \
		echo "CMake не найден. Устанавливаю..."; \
		sudo apt update && sudo apt install -y cmake; \
	fi

install-boost:
	@sudo apt update
	@sudo apt install -y libboost-all-dev

install-spdlog:
	@sudo apt update
	@sudo apt install -y libspdlog-dev

install-toml:
	@sudo apt update
	@sudo apt install -y libtomlplusplus-dev

install-deps: install-cmake install-boost install-spdlog install-toml
	@echo "Все библиотеки и зависимости установлены!"

# Быстрая сборка
fast:
	@echo "Сборка происходит на $(CORES) ядрах:"
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	@cmake --build build -j$(CORES)

# Сборка и запуск
run: fast
	@./build/csv_median_calculator --cfg ./config.toml

#Проверка на утечки
valgrind:
	valgrind --leak-check=full ./build/csv_median_calculator --cfg ./config.toml

addressSanitizer:
	@echo "Сборка происходит с addressSanitizer:"
	@cmake -S . -B build -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug
	@cmake --build build -j$(CORES)

# Очистка
clean:
	@rm -rf build

# Пересборка
rebuild: clean fast