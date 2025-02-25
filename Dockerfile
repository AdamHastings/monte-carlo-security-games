# Use Ubuntu 22.04 as the base image
FROM ubuntu:22.04

# Set noninteractive mode for apt-get to avoid prompts
ENV DEBIAN_FRONTEND=noninteractive

# Update package lists and install development tools:
# - build-essential: provides gcc, g++, make, etc.
# - g++: the GNU C++ compiler (ensure it's recent enough for C++20)
# - python3 and python3-pip: for running Python code and managing packages
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    python3 \
    python3-pip \
    python3-numpy \
    python3-pandas \
    python3-plotly \
    python3-matplotlib \
    libgsl-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY parameter-calcs/ /root/parameter-calcs/
COPY simulator/ /root/simulator/
RUN ls -la /

# Default to an interactive shell
CMD ["bash"]