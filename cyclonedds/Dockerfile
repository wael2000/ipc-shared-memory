# Use a minimal base image, specifically UBI8 as it is stable.
FROM registry.access.redhat.com/ubi8/ubi-minimal:8.10

# Set environment variables for OpenSplice installation
ENV OSPL_HOME=/opt/cyclonedds \
    CYCLONEDDS_URI=file:///etc/cyclonedds/cyclonedds.xml

# Install all build-time and runtime dependencies in a single layer.
RUN microdnf install -y \
    shadow-utils \
    tar \
    bash \
    git \
    cmake \
    make \
    gcc-c++ \
    && microdnf clean all \
    && rm -rf /var/cache/yum

# Copy the application source code and build it
COPY app/publisher.cpp /app/publisher.cpp
COPY app/subscriber.cpp /app/subscriber.cpp

# Install Cyclone DDS from source and then compile the application.
# The issue is that g++ on the same RUN command does not inherit the
# path settings from the `make install` command.
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds && \
    mkdir -p /tmp/cyclonedds/build && \
    cd /tmp/cyclonedds/build && \
    cmake .. && \
    make && \
    # The 'make install' command places headers in /usr/local/include
    # and libraries in /usr/local/lib
    make install && \
    cd /app && \
    # The -I and -L flags are correct, but we must make sure the build
    # environment is pointing to the correct locations.
    g++ publisher.cpp -o publisher \
        -I/usr/local/include \
        -L/usr/local/lib \
        -lcyclonedds -lstdc++ -lpthread -lm -ldl -std=c++11 && \
    rm -rf /tmp/cyclonedds
    
# Copy the configuration file
COPY cyclonedds.xml /etc/cyclonedds/

# Set a non-root user
RUN groupadd -r cyclonedds && useradd -r -g cyclonedds cyclonedds
USER cyclonedds

# Define the ENTRYPOINT
#ENTRYPOINT ["/app/publisher"]