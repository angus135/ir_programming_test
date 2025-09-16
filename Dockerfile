FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    pkg-config \
    valgrind \
    cmake \
    ca-certificates \
    wget \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /project
COPY . /project

# default build target
CMD ["make", "test"]

## Dockerfile generated using Generative AI