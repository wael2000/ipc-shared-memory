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
    // Create shared memory object
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Configure size
    ftruncate(shm_fd, SIZE);

    // Map to memory
    void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Write message
    const char *message = "Hello from Writer (Container 1)!";
    std::memcpy(ptr, message, strlen(message) + 1);

    print_namespaces();
    std::cout << "Writer wrote: " << message << std::endl;

    // Keep alive so reader can read
    sleep(30);

    return 0;
}
