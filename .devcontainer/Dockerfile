FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive


RUN apt update && apt -y install \
    cmake \
    curl \
    git \
    build-essential


RUN apt update && apt upgrade -y && apt install -y --no-install-recommends curl locales ca-certificates 
RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen en_US.utf8 && /usr/sbin/update-locale 
ENV LANG=en_US.UTF-8

# ## fix certificates issues when in WSL/ubuntu/docker
# COPY certificates/*.crt  /usr/local/share/ca-certificates/
# RUN chmod 644 /usr/local/share/ca-certificates/*.crt && update-ca-certificates


RUN apt update && apt install -y gdb valgrind lcov jq

############# user stuff ###########
RUN apt update && apt install -y bash-completion make

ENV USER=builder
RUN useradd -s /bin/bash -m $USER
RUN usermod -a -G staff $USER

RUN apt install -y sudo
RUN usermod -a -G sudo $USER
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers


USER $USER
WORKDIR /home/$USER