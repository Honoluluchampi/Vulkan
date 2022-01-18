CFLAGS = -std=c++17 -O2
FILENAME = 01_creating_an_instance.cpp
SOURCEDIR = src
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
vulkan: $(SOURCEDIR)/$(FILENAME)
	g++ $(CFLAGS) -o vulkan $(SOURCEDIR)/$(FILENAME) $(LDFLAGS)

.PHONY: test clean

test: vulkan
	./vulkan

clean: rm -f vulkan