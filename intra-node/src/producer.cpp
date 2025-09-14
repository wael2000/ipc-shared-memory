#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>

const char *shm_name = "/my_shm";
const size_t SIZE = 4096;

std::string get_namespace_id(const std::string& ns_type) {
    std::string ns_path = "/proc/self/ns/" + ns_type;
    try {
        auto link = std::filesystem::read_symlink(ns_path);
        return link.string();  // e.g. "ipc:[4026531839]"
    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
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
    std::string full_message = std::string(message) + " | PID=" + get_namespace_id("pid");

    std::memcpy(ptr, full_message.c_str(), full_message.size() + 1);

    // std::memcpy(ptr, message, strlen(message) + 1);

    std::cout << "Writer wrote: " << message << std::endl;

    // Keep alive so reader can read
    sleep(30);

    return 0;
}
