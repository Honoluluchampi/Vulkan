CFLAGS = -std=c++17 -O2
FILENAME = 01_creating_an_instance.cpp
SOURCEDIR = src
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
Test: $(SOURCEDIR)/$(FILENAME)
	g++ $(CFLAGS) -o test $(SOURCEDIR)/$(FILENAME) $(LDFLAGS)

.PHONY: test clean

test: test
	./test

clean: rm -f test