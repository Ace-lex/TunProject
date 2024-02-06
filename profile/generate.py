# Generate test files for performance analysis
import os
import random

SUFFIX = ".bin"

# Generate random files of fixed size
def generate_random_binary_file(file_path, size_kb):
    with open(file_path, 'wb') as file:
        random_data = bytearray(random.getrandbits(8) for _ in range(size_kb * 1024))
        file.write(random_data)

# Generate test files
def generate_perf_directory(directory_prefix, size_kb, num_of_file):
    directory_name = f"{directory_prefix}_{size_kb}kb"
    if not os.path.exists(directory_name):
        os.makedirs(directory_name)
    for i in range(num_of_file):
        file_name = f'{directory_name}/{directory_name}{i}{SUFFIX}'
        generate_random_binary_file(file_name, size_kb)


if __name__ == "__main__":
    FILE_NUM = 20

    PROF_PREFIX = "prof"
    # Generate 1kb payloads
    generate_perf_directory(PROF_PREFIX, 1, FILE_NUM)

    # Generate 4kb payloads
    generate_perf_directory(PROF_PREFIX, 4, FILE_NUM)

    # Generate 8kb payloads
    generate_perf_directory(PROF_PREFIX, 8, FILE_NUM)

    # Generate 16kb payloads
    generate_perf_directory(PROF_PREFIX, 16, FILE_NUM)

    # Generate 32kb payloads
    generate_perf_directory(PROF_PREFIX, 32, FILE_NUM)
