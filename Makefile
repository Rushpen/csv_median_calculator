.PHONY: all fast run clean

# Количество ядер процессора
CORES := $(shell nproc 2>/dev/null || echo 4)

# Самая быстрая сборка
fast:
	@echo "Сборка происходит на $(CORES) ядрах:"
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	@cmake --build build -j$(CORES)

# Сборка и запуск
run: fast
	@./build/csv_median_calculator -c ./config.toml

# Очистка
clean:
	@rm -rf build