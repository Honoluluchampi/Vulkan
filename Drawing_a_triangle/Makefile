TARGET = HelloTriangleApplication
COMPILER = g++
CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
#FILENAME = main.cpp
SOURCEDIR = ./src
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
LIBS = 
INCLUDE = 
OBJECTDIR = ./obj
OBJECTS = $(addprefix $(OBJECTDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	-mkdir -p $(OBJECTDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

test:
	./$(TARGET)

clean:
	-rm -f $(OBJECTDIR)
	-rm -f $(notdir $(TARGET)) $(DEPENDS)

-include $(DEPENDS)