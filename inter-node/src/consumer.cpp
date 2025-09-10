#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>

const char *shm_name   = "/dds_shm";
const char *sem_empty  = "/dds_sem_empty";
const char *sem_full   = "/dds_sem_full";
const size_t SIZE      = 1024;

const int listen_port = 5000;

void wait_for_signal() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return;
    }

    char buffer[64];
    recv(sock, buffer, sizeof(buffer), 0); // blocking wait
    close(sock);
}

int main() {
    // Open shared memory
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open");
        return 1;
    }

    void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Open semaphores
    sem_t *empty = sem_open(sem_empty, 0);
    sem_t *full  = sem_open(sem_full, 0);

    while (true) {
        // Wait for remote signal first (inter-Pod sync)
        wait_for_signal();

        // Then local semaphore to ensure safe read
        sem_wait(full);

        std::cout << "[Consumer] Received: " << static_cast<char*>(ptr) << std::endl;

        sem_post(empty);
    }

    return 0;
}
