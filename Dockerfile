FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    python3 \
    python3-pip \
    maven \
    default-jdk \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Clone and build CycloneDDS
RUN git clone https://github.com/eclipse-cyclonedds/cyclonedds.git && \
    cd cyclonedds && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/cyclonedds && \
    cmake --build . --target install

# Set environment variables for CycloneDDS
ENV CYCLONEDDS_HOME=/usr/local/cyclonedds
ENV PATH=/usr/local/cyclonedds/bin:$PATH
ENV LD_LIBRARY_PATH=/usr/local/cyclonedds/lib

# Create application directory
RUN mkdir -p /app/cyclonedds-sender

# Copy the application source (excluding any build artifacts)
COPY CMakeLists.txt /app/cyclonedds-sender/
COPY src/ /app/cyclonedds-sender/src/
COPY include/ /app/cyclonedds-sender/include/
COPY cyclonedds.xml /app/cyclonedds-sender/

# Build the application
WORKDIR /app/cyclonedds-sender
RUN mkdir -p build && \
    cd build && \
    cmake .. -DCYCLONEDDS_HOME=/usr/local/cyclonedds && \
    cmake --build .

# Run the application
CMD ["/app/cyclonedds-sender/build/cyclonedds_sender"]
