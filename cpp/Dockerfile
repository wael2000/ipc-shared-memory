# Use GCC as base
FROM gcc:latest

# Set working directory
WORKDIR /app

# Copy source code into container
COPY src/producer.cpp src/consumer.cpp ./

# Build both executables
RUN g++ -o producer producer.cpp -pthread && \
    g++ -o consumer consumer.cpp -pthread

# Default to bash so you can run producer or consumer manually
CMD ["/bin/bash"]
