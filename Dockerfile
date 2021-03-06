FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ gcc git wget make cmake mesa-utils x11-apps
RUN apt-get update && apt-get install -y --no-install-recommends \
    vulkan-tools libvulkan-dev \
    vulkan-validationlayers-dev spirv-tools \
    mesa-vulkan-drivers vulkan-utils \
    # glfw(window api), glm(linear algebra for cg)
    libglfw3-dev libglm-dev \
    # dependency for glfw
    libxi-dev libxxf86vm-dev libxrandr-dev

ENV DISPLAY host.docker.internal:0