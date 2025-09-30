#include <iostream>
#include "dds/dds.hpp"

// Define the same struct as the publisher
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
    dds::sub::Subscriber subscriber(dp);
    dds::sub::DataReader<HelloWorld> reader(subscriber, topic);

    std::cout << "Subscriber is waiting for a publisher..." << std::endl;

    // Wait for data
    while (true) {
        auto samples = reader.take();
        for (const auto& sample : samples) {
            if (sample.info().valid()) {
                const auto& message = sample.data();
                std::cout << "Subscriber received: '" << message.message << "'" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}