#!/usr/bin/env sh

set -e

CONTAINER=orange-tcp
WORKSPACE=orange-tcp
TARGET=$1

./build.sh $TARGET
echo "Build complete.\n"
./run.sh $TARGET
