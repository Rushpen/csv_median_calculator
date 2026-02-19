import csv
import random
import os
from datetime import datetime, timedelta

# Конфигурация
BASE_TIME = 1716810808593627  # начальный timestamp в микросекундах(сколько прошло с 1 января 1970 года)
PRICE_BASE = 68480.0
OUTPUT_DIR = "../examples/input"

# Создаем директорию, если её нет
os.makedirs(OUTPUT_DIR, exist_ok=True)

def generate_level_file(filename, num_rows=100):
    """Генерирует файл level.csv с данными"""
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(['receive_ts', 'exchange_ts', 'price', 'quantity', 'side', 'rebuild'])
        
        for i in range(num_rows):
            # Немного сдвигаем timestamp для реалистичности
            ts_offset = i * 1000  # 1 мс между записями
            receive_ts = BASE_TIME + ts_offset
            exchange_ts = receive_ts - random.randint(10, 100)  # биржа чуть раньше
            
            # Генерируем цены с небольшими колебаниями
            price_change = random.uniform(-10, 10)
            price = PRICE_BASE + price_change
            
            # Чередуем bid/ask
            side = 'bid' if random.random() > 0.5 else 'ask'
            
            # Случайный объем
            quantity = round(random.uniform(0.001, 15.0), 6)
            
            # Редкий флаг перестроения
            rebuild = 1 if random.random() < 0.1 else 0
            
            writer.writerow([
                receive_ts,
                exchange_ts,
                f"{price:.8f}",
                f"{quantity:.6f}",
                side,
                rebuild
            ])

def generate_trade_file(filename, num_rows=100):
    """Генерирует файл trade.csv с данными сделок"""
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f, delimiter=';')
        
        # Заголовок
        writer.writerow(['receive_ts', 'exchange_ts', 'price', 'quantity', 'side'])
        
        for i in range(num_rows):
            ts_offset = i * 500  # сделки могут быть чаще
            receive_ts = BASE_TIME + ts_offset
            exchange_ts = receive_ts - random.randint(5, 50)
            
            # Цены сделок обычно чуть более волатильны
            price_change = random.uniform(-15, 15)
            price = PRICE_BASE + price_change
            
            # Объемы сделок обычно меньше
            quantity = round(random.uniform(0.001, 5.0), 6)
            
            side = 'bid' if random.random() > 0.5 else 'ask'
            
            writer.writerow([
                receive_ts,
                exchange_ts,
                f"{price:.8f}",
                f"{quantity:.6f}",
                side
            ])

# Генерируем файлы
print("Генерация level.csv...")
generate_level_file(f"{OUTPUT_DIR}/btcusdt_level_2024.csv", 200)

print("Генерация trade.csv...")
generate_trade_file(f"{OUTPUT_DIR}/btcusdt_trade_2024.csv", 150)

print(f"Файлы созданы в директории {OUTPUT_DIR}")