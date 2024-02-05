import os
import random

SUFFIX = ".bin"

def generate_random_binary_file(file_path, size_kb):
    with open(file_path, 'wb') as file:
        random_data = bytearray(random.getrandbits(8) for _ in range(size_kb * 1024))
        file.write(random_data)

def generate_perf_directory(directory_name, size_kb, num_of_file):
    if not os.path.exists(directory_name):
        os.makedirs(directory_name)
    for i in range(num_of_file):
        file_name = f'{directory_name}/{directory_name}{i}{SUFFIX}'
        generate_random_binary_file(file_name, size_kb)


if __name__ == "__main__":
    FILE_NUM = 20

    PROF_TEST1 = "prof_1kb"
    PROF_TEST2 = "prof_4kb"
    PROF_TEST3 = "prof_8kb"
    PROF_TEST4 = "prof_16kb"

    # Generate 1kb payloads
    generate_perf_directory(PROF_TEST1, 1, FILE_NUM)

    # Generate 4kb payloads
    generate_perf_directory(PROF_TEST2, 4, FILE_NUM)

    # Generate 8kb payloads
    generate_perf_directory(PROF_TEST3, 8, FILE_NUM)

    # Generate 16kb payloads
    generate_perf_directory(PROF_TEST4, 16, FILE_NUM)
