TARGET = HelloTriangleApplication
COMPILER = g++
CFLAGS = -std=c++20 -O2 -g3
VULKANDIR = /Users/toyotariku/VulkanSDK/1.2.198.1/macOS
GLFWDIR = /usr/local/Cellar/glfw/3.3.6
LDFLAGS = $(VULKANDIR)/lib/libvulkan.1.2.198.dylib $(VULKANDIR)/lib/libvulkan.1.dylib $(GLFWDIR)/lib/libglfw.3.3.dylib
SOURCEDIR = ./src
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
LIBS = 
INCLUDE = -I/usr/local/include -I$(VULKANDIR)/include
OBJECTDIR = ./obj
OBJECTS = $(addprefix $(OBJECTDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS = $(OBJECTS:.o=.d)

VK_ICD_FILENAMES := $(VULKANDIR)/share/vulkan/icd.d/MoltenVK_icd.json
VK_LAYER_PATH := $(VULKANDIR)/share/vulkan/explicit_layer.d

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.cpp
	-mkdir -p $(OBJECTDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

test:
	./$(TARGET)

clean:
	-rm -rf $(OBJECTDIR)
	-rm -f $(notdir $(TARGET)) $(DEPENDS)

-include $(DEPENDS)