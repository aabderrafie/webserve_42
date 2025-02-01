
# Use a base image
FROM ubuntu:latest

# Install necessary packages
RUN apt-get update && apt-get install -y \
    sudo \
    passwd \
    curl \
    gnupg \
    lsb-release \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Install Docker CLI
RUN curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add - && \
    add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" && \
    apt-get update && \
    apt-get install -y docker-ce-cli

# Add a user and give it sudo privileges
RUN useradd -m -s /bin/bash eva && echo 'eva:eva' | chpasswd && usermod -aG sudo eva

# Set the default command to run a bash shell
CMD ["/bin/bash"]