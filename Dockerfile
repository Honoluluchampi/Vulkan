FROM ubuntu:20.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ gcc git wget make mesa-utils x11-apps
RUN apt-get update && apt-get install -y --no-install-recommends \
    vulkan-tools libvulkan-dev \
    vulkan-validationlayers-dev spirv-tools

ENV DISPLAY host.docker.internal:0