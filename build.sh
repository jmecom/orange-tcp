#!/usr/bin/env sh

CONTAINER=orange-tcp
TARGET=$1
docker cp src $CONTAINER:/
docker exec -w /src $CONTAINER bazel build //:$1
