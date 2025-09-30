# Use a minimal base image with a C++ compiler environment
FROM registry.access.redhat.com/ubi8/ubi-minimal

# Install required dependencies: a C++ compiler and Cyclone DDS
RUN microdnf update -y && \
    microdnf install -y gcc-c++ tar && \
    microdnf install -y git cmake make && \
    # Install Cyclone DDS from source
    git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds && \
    mkdir -p /tmp/cyclonedds/build && \
    cd /tmp/cyclonedds/build && \
    cmake .. && \
    make && \
    make install && \
    # Clean up build dependencies
    microdnf remove -y git cmake make gcc-c++ && \
    microdnf clean all && \
    rm -rf /tmp/cyclonedds

# Copy the application source code and build it
COPY publisher.cpp /app/publisher.cpp
COPY subscriber.cpp /app/subscriber.cpp
RUN g++ /app/publisher.cpp -o /app/publisher -I/usr/local/include -L/usr/local/lib -lcyclonedds -std=c++11

# Copy the configuration file
COPY cyclonedds.xml /etc/cyclonedds/

# Set a non-root user
RUN groupadd -r cyclonedds && useradd -r -g cyclonedds cyclonedds
USER cyclonedds

# Set the environment variable for the configuration file
ENV CYCLONEDDS_URI=file:///etc/cyclonedds/cyclonedds.xml

# Command to run the publisher application
# CMD ["/app/publisher"]