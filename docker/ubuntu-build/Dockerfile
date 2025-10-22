FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Build deps
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential cmake  git ca-certificates \
    libgl1-mesa-dev libglu1-mesa-dev libglfw3 libglfw3-dev \
    lua5.4 liblua5.4-dev \
 && rm -rf /var/lib/apt/lists/*

# # Optional: create non-root user
# RUN useradd -m builder
# WORKDIR /home/builder
# USER builder
WORKDIR /root

# Clone and build (adjust repo URL if needed)
RUN git clone https://github.com/lbinria/salamesh.git
# WORKDIR /home/builder/salamesh
WORKDIR /root/salamesh

# Configure and build
RUN cmake -B build \
 && cmake --build build --parallel

# Default command: run built binary if present (adjust executable name if different)
# CMD ["./build/salamesh"]