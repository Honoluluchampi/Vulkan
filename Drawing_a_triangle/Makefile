CFLAGS = -std=c++17 -O2
FILENAME = main.cpp
SOURCEDIR = src
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
vulkan: $(SOURCEDIR)/$(FILENAME)
	g++ $(CFLAGS) -o vulkan $(SOURCEDIR)/$(FILENAME) $(LDFLAGS)

.PHONY: test clean

test: vulkan
	VK_LAYER_PATH=/usr/include/vulkan
		./vulkan

clean: rm -f vulkan