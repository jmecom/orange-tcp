#!/usr/bin/env sh

FROM=orange-tcp
WORKSPACE=orange-tcp
TO=$1
TARGET=$2

docker cp $FROM:/$WORKSPACE/bazel-bin/$TARGET /tmp/$TARGET
docker cp /tmp/$TARGET $TO:/
rm -f /tmp/$TARGET
