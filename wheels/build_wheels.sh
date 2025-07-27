#! /bin/bash

ARCH=$(uname -m)

if [ "$ARCH" == "x86_64" ]; then
    DOCKERFILE="wheels/Dockerfile.x86_64"
elif [ "$ARCH" == "arm64" ] || [ "$ARCH" == "aarch64" ]; then
    DOCKERFILE="wheels/Dockerfile.aarch64"
else
    echo "Unsupported architecture: $ARCH"
    exit 1
fi
set -e
root_dir=$(dirname $(dirname $(realpath $0)))

cd $root_dir
PYPEERTALK_COMMIT=$(git rev-parse HEAD)
docker build -t pypeertalk wheels --build-arg PYPEERTALK_COMMIT=$PYPEERTALK_COMMIT --file $DOCKERFILE
docker create --name pypeertalk-container pypeertalk
docker cp pypeertalk-container:/root/pypeertalk/wheelhouse wheels/
docker rm pypeertalk-container

