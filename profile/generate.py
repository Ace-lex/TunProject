import os
import random

def generate_random_binary_file(file_path, size_kb):
    with open(file_path, 'wb') as file:
        random_data = bytearray(random.getrandbits(8) for _ in range(size_kb * 1024))
        file.write(random_data)

if __name__ == "__main__":
    # Generate small payloads
    if not os.path.exists("small"):
        os.makedirs("small")
    for i in range(20):
        file_name = f'small/small{i}.bin'
        generate_random_binary_file(file_name, 1)
        # print(f'{file_name} has generated')

    # Generate medium payloads
    if not os.path.exists("medium"):
        os.makedirs("medium")
    for i in range(20):
        file_name = f'medium/medium{i}.bin'
        generate_random_binary_file(file_name, 4)
        # print(f'{file_name} has generated')

    # Generate large payloads
    if not os.path.exists("large"):
        os.makedirs("large")
    for i in range(20):
        file_name = f'large/large{i}.bin'
        generate_random_binary_file(file_name, 8)
        # print(f'{file_name} has generated')
