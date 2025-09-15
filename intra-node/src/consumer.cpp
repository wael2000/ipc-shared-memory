#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>   // for namespace info

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
    
    // Read message from shared memory
    std::string producer_message = static_cast<char*>(ptr);

    // Append this consumer's IPC namespace info
    std::string full_message = producer_message + 
                               " [ Reader IPC=" + get_namespace_id("ipc") +
                               " | PID=" + get_namespace_id("pid") + 
                               " ] ";

    std::cout << "Reader read: " << full_message << std::endl;


    return 0;
}
