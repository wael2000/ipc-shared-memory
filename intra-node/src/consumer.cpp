#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>

const char *shm_name = "/my_shm";
const size_t SIZE = 4096;


void print_namespaces() {
    std::cout << "=== Namespace Info ===" << std::endl;
    for (const auto& entry : std::filesystem::directory_iterator("/proc/self/ns")) {
        std::cout << entry.path().filename() << " -> " 
                  << std::filesystem::read_symlink(entry.path()) << std::endl;
    }
    std::cout << "======================" << std::endl;
    
}

int main() {
    // Open existing shared memory
    int shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Map to memory
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    print_namespaces();
    // Read message
    std::cout << "Reader read: " << static_cast<char*>(ptr) << std::endl;

    return 0;
}
