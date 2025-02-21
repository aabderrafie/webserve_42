# Use Ubuntu as the base image
FROM ubuntu:22.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic utilities and dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    git \
    wget \
    curl \
    gcc \
    gdb \
    make \
    openssh-server \
    zsh \
    vim \
    netcat \
    socat \
    strace \
    ltrace \
    binwalk \
    foremost \
    steghide \
    john \
    hashcat \
    && rm -rf /var/lib/apt/lists/*

# Install Python packages for exploitation
RUN python3 -m pip install --no-cache-dir \
    pwntools \
    ROPgadget \
    capstone \
    unicorn \
    keystone-engine \
    z3-solver

# Install Ghidra (for reverse engineering)
RUN mkdir -p /opt/ghidra && \
    wget -q https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_10.3.3_build/ghidra_10.3.3_PUBLIC_20230829.zip -O /tmp/ghidra.zip && \
    unzip /tmp/ghidra.zip -d /opt/ghidra && \
    rm /tmp/ghidra.zip

# Configure SSH
RUN mkdir /var/run/sshd
RUN echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config

# Install and configure Oh My Zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended
RUN chsh -s $(which zsh)

# Create working directory
WORKDIR /ctf

# Expose SSH port
EXPOSE 22

# Start SSH service and Zsh
CMD service ssh start && zsh

# Usage instructions in comments:
# Build: docker build -t ctf-environment .
# Run: docker run -d -v $(pwd):/ctf -p 2222:22 ctf-environment
# SSH: ssh -p 2222 root@localhost