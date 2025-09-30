# Use a minimal base image, specifically UBI8 as it is stable.
FROM registry.access.redhat.com/ubi8/ubi-minimal:8.10

# Set environment variables for OpenSplice installation
ENV OSPL_HOME=/opt/cyclonedds \
    CYCLONEDDS_URI=file:///etc/cyclonedds/cyclonedds.xml

# Install all build-time and runtime dependencies in a single layer.
# This avoids most dependency-resolution issues.
#
# 1. shadow-utils: Provides useradd/groupadd.
# 2. tar, bash: Required for the build script.
# 3. git, cmake, make, gcc-c++: The build tools for Cyclone DDS.
#
# We skip the 'update' command to avoid conflicting with the base image's pre-installed packages.
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

# Install Cyclone DDS from source
# Using a separate RUN command for the build process is fine,
# as the dependencies are now handled.
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds \
    && mkdir -p /tmp/cyclonedds/build \
    && cd /tmp/cyclonedds/build \
    && cmake .. \
    && make \
    && make install \
    && g++ /app/publisher.cpp -o /app/publisher -I/usr/local/include -L/usr/local/lib -lcyclonedds -std=c++11 \
    && rm -rf /tmp/cyclonedds

# Copy the configuration file
COPY cyclonedds.xml /etc/cyclonedds/

# Set a non-root user
RUN groupadd -r cyclonedds && useradd -r -g cyclonedds cyclonedds
USER cyclonedds

# Define the ENTRYPOINT, which is the path to our executable.
#ENTRYPOINT ["/app/publisher"]