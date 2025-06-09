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
    libyaml-cpp-dev \
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
RUN mkdir -p /app/IDL2DDS

# Copy the application source (excluding any build artifacts)
COPY CMakeLists.txt /app/IDL2DDS/
COPY src/ /app/IDL2DDS/src/
COPY include/ /app/IDL2DDS/include/
COPY test-cases/ /app/IDL2DDS/test-cases/
COPY cyclonedds.xml /app/IDL2DDS/
COPY generate_handler.sh /app/IDL2DDS/
COPY ensure_newlines.sh /app/IDL2DDS/
COPY auto_generate_handlers.sh /app/IDL2DDS/

# Make scripts executable
WORKDIR /app/IDL2DDS
RUN chmod +x generate_handler.sh ensure_newlines.sh auto_generate_handlers.sh

# Fix newlines in IDL files and auto-generate handlers for new IDL files
RUN ./ensure_newlines.sh
RUN ./auto_generate_handlers.sh

# Ensure the proper system environment is set up
ENV PATH=/usr/local/cyclonedds/bin:$PATH
ENV LD_LIBRARY_PATH=/usr/local/cyclonedds/lib:$LD_LIBRARY_PATH

# Build the application
RUN mkdir -p build && \
    cd build && \
    cmake .. -DCYCLONEDDS_HOME=/usr/local/cyclonedds && \
    make -j4

# Set default command
CMD ["/app/IDL2DDS/build/IDL2DDS", "/app/IDL2DDS/test-cases/test-message.yaml"]
