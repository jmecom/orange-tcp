#!/usr/bin/env sh

docker-machine restart
eval $(docker-machine env default)
docker run -v $PWD:/pwd --cap-add=SYS_PTRACE --cap-add NET_ADMIN --privileged --security-opt seccomp=unconfined -d --name orange-tcp -i orange-tcp:ubuntu20.10
docker exec -it orange-tcp /bin/bash
