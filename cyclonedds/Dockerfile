# ========================
# STAGE 1: THE BUILDER
# ========================
# Use a full UBI image for the build environment, as it has dnf and all tools.
FROM registry.access.redhat.com/ubi8/ubi AS builder

# Install all build-time dependencies.
RUN dnf install -y git cmake make gcc-c++ tar bash shadow-utils && dnf clean all

# Copy the application source code.
WORKDIR /app
COPY publisher.cpp .
COPY subscriber.cpp .

# Set a persistent environment variable for the install prefix
ENV CYCLONEDDS_PREFIX=/usr/local

# Clone, build, and install Cyclone DDS.
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds && \
    mkdir -p /tmp/cyclonedds/build && \
    cd /tmp/cyclonedds/build && \
    cmake -DCMAKE_INSTALL_PREFIX=${CYCLONEDDS_PREFIX} .. && \
    make && \
    make install && \
    # Compile the application using the installed headers and libraries.
    # We explicitly include the install prefix in the command.
    g++ /app/publisher.cpp -o /app/publisher \
        -I${CYCLONEDDS_PREFIX}/include \
        -L${CYCLONEDDS_PREFIX}/lib \
        -lcyclonedds -lstdc++ -lpthread -lm -ldl -std=c++11 && \
    # Clean up the build environment to save space in this stage
    rm -rf /tmp/cyclonedds

# ========================
# STAGE 2: THE FINAL IMAGE
# ========================
# Use a minimal UBI image for the final, lean runtime.
FROM registry.access.redhat.com/ubi8/ubi-minimal

# Install any runtime dependencies that CycloneDDS might need.
# As of now, it's just basic libraries, but this is a good practice.
RUN microdnf install -y \
    shadow-utils \
    && microdnf clean all

# Copy the final executable from the builder stage.
COPY --from=builder /app/publisher /app/publisher

# Copy the CycloneDDS libraries from the builder stage.
COPY --from=builder /usr/local/lib/libcyclonedds.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libddsc.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libdds-cxx.so* /usr/local/lib/

# Copy the configuration file.
COPY cyclonedds.xml /etc/cyclonedds/

# Set a non-root user for security.
RUN groupadd -r cyclonedds && useradd -r -g cyclonedds cyclonedds
USER cyclonedds

# Set environment variables for the runtime.
ENV CYCLONEDDS_URI=file:///etc/cyclonedds/cyclonedds.xml
# This is crucial for the dynamic linker at runtime.
ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

# Define the ENTRYPOINT for the container.
#ENTRYPOINT ["/app/publisher"]