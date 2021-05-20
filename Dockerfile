# Instructions:
# docker build -t orange-tcp:ubuntu20.10 .
# docker run --rm -v $PWD:/pwd --cap-add=SYS_PTRACE --cap-add SYS_NET_ADMIN --security-opt seccomp=unconfined -d --name orange-tcp -i ctf:ubuntu20.10
# docker exec -it orange-tcp /bin/bash

FROM ubuntu:20.10
ENV LC_CTYPE C.UTF-8
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture i386 && \
apt-get update && \
apt-get install -y build-essential strace ltrace curl wget \
clang dnsutils netcat gcc-multilib net-tools vim gdb gdb-multiarch python3 python3-pip python3-dev \
libssl-dev libffi-dev wget git make procps libpcre3-dev libdb-dev libxt-dev libxaw7-dev libc6:i386 libncurses5:i386 libstdc++6:i386 bpython
