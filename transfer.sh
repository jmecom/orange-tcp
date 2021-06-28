#!/usr/bin/env sh

FROM=orange-tcp
TO=orange-tcp-2
WORKSPACE=orange-tcp
TARGET=$1

docker cp $FROM:/$WORKSPACE/bazel-bin/$TARGET /tmp/$TARGET
docker cp /tmp/$TARGET $TO:/
rm -f /tmp/$TARGET
