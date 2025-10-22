FROM archlinux

ENV DEBIAN_FRONTEND=noninteractive

# Build deps
RUN pacman -Sy \
# RUN pacman -Syu \
   && pacman -S --noconfirm base-devel cmake git mesa glfw lua
#    base-devel cmake git mesa glfw lua \
#    && pacman -Scc --noconfirm

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