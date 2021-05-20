#!/usr/bin/env sh

CONTAINER=orange-tcp
WORKSPACE=orange-tcp
TARGET=$1

docker cp $WORKSPACE $CONTAINER:/
docker exec -w /$WORKSPACE $CONTAINER bazel build --config=asan //:$1
