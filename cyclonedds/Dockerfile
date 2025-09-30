# ========================
# STAGE 1: THE BUILDER
# ========================
FROM registry.access.redhat.com/ubi8/ubi AS builder

# Install all build-time dependencies.
RUN dnf install -y git cmake make gcc-c++ tar bash shadow-utils && dnf clean all

# Set a persistent environment variable for the install prefix
ENV CYCLONEDDS_PREFIX=/usr/local

# Clone and build the core CycloneDDS (C implementation).
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds && \
    mkdir -p /tmp/cyclonedds/build && \
    cd /tmp/cyclonedds/build && \
    cmake -DCMAKE_INSTALL_PREFIX=${CYCLONEDDS_PREFIX} .. && \
    make && \
    make install

# Clone and build the CycloneDDS C++ API.
# It needs to know where the core DDS library is located.
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git /tmp/cyclonedds-cxx && \
    mkdir -p /tmp/cyclonedds-cxx/build && \
    cd /tmp/cyclonedds-cxx/build && \
    cmake -DCMAKE_INSTALL_PREFIX=${CYCLONEDDS_PREFIX} -D DDS_DIR=/tmp/cyclonedds/build .. && \
    make && \
    make install

# Copy the application source code.
WORKDIR /app
COPY publisher.cpp .
COPY subscriber.cpp .

# Compile the application using both C and C++ API libraries.
# We now link against both `libcyclonedds` and `libddscxx`.
RUN g++ publisher.cpp -o publisher \
    -I${CYCLONEDDS_PREFIX}/include \
    -L${CYCLONEDDS_PREFIX}/lib \
    -lcyclonedds -lddscxx -lstdc++ -lpthread -lm -ldl -std=c++11 && \
    rm -rf /tmp/cyclonedds /tmp/cyclonedds-cxx

# ========================
# STAGE 2: THE FINAL IMAGE
# ========================
FROM registry.access.redhat.com/ubi8/ubi-minimal

# Install any runtime dependencies.
RUN microdnf install -y shadow-utils && microdnf clean all

# Copy the final executable from the builder stage.
COPY --from=builder /app/publisher /app/publisher

# Copy the required shared libraries from the builder stage.
# We now need `libcyclonedds.so` AND `libddscxx.so`.
COPY --from=builder /usr/local/lib/libcyclonedds.so* /usr/local/lib/
COPY --from=builder /usr/local/lib/libddscxx.so* /usr/local/lib/

# Copy the configuration file.
COPY cyclonedds.xml /etc/cyclonedds/

# Set a non-root user for security.
RUN groupadd -r cyclonedds && useradd -r -g cyclonedds cyclonedds
USER cyclonedds

# Set environment variables for the runtime.
ENV CYCLONEDDS_URI=file:///etc/cyclonedds/cyclonedds.xml
ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

# Define the ENTRYPOINT for the container.
#ENTRYPOINT ["/app/publisher"]