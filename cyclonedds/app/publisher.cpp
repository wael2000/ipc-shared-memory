#include <iostream>
#include <chrono>
#include <thread>
#include "dds/dds.hpp"

// Define a simple struct for our message data
struct HelloWorld {
    int id;
    std::string message;
};

// Create a DDS topic type for our struct
ORB_DEFINE_TYPE(HelloWorld, "HelloWorldTopic");

int main() {
    // DDS initialization
    dds::domain::DomainParticipant dp(0);
    dds::topic::Topic<HelloWorld> topic(dp, "HelloWorldTopic");
    dds::pub::Publisher publisher(dp);
    dds::pub::DataWriter<HelloWorld> writer(publisher, topic);

    std::cout << "Publisher is waiting for a subscriber..." << std::endl;

    // Wait for a matching subscriber to be discovered
    dds::core::status::PublicationMatchedStatus pub_status;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        writer.get_publication_matched_status(pub_status);
    } while (pub_status.current_count() < 1);

    int count = 0;
    while (true) {
        HelloWorld message;
        message.id = count;
        message.message = "Hello, World! " + std::to_string(count);
        
        std::cout << "Publishing message: '" << message.message << "'" << std::endl;
        writer.write(message);

        count++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}