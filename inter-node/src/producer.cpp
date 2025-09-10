#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const char *shm_name   = "/dds_shm";
const char *sem_empty  = "/dds_sem_empty";
const char *sem_full   = "/dds_sem_full";
const size_t SIZE      = 1024;

// Consumer Service IP/Port (set to your Kubernetes Service)
const char *consumer_ip   = "consumer-svc";  // replace with service DNS, e.g., "consumer-svc"
const int consumer_port   = 5000;

void send_signal() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(consumer_port);
    inet_pton(AF_INET, consumer_ip, &addr.sin_addr);

    const char *msg = "data_ready";
    sendto(sock, msg, strlen(msg), 0, (sockaddr*)&addr, sizeof(addr));
    close(sock);
}

int main() {
    // Create shared memory
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    void *ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Create semaphores
    sem_t *empty = sem_open(sem_empty, O_CREAT, 0666, 1); // initially empty
    sem_t *full  = sem_open(sem_full, O_CREAT, 0666, 0); // initially not full

    int count = 0;
    while (true) {
        sem_wait(empty);

        std::string msg = "Hello DDS subscriber, message #" + std::to_string(++count);
        std::memcpy(ptr, msg.c_str(), msg.size() + 1);

        std::cout << "[Producer] Sent: " << msg << std::endl;

        sem_post(full);   // local synchronization
        send_signal();    // notify remote consumers (via UDP)

        sleep(2); // simulate work
    }

    return 0;
}
