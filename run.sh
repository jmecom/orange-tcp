#!/usr/bin/env sh

CONTAINER=orange-tcp
WORKSPACE=orange-tcp
TARGET=$1

docker exec -w /$WORKSPACE $CONTAINER bazel-bin/$1
