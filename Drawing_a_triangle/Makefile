TARGET = HelloTriangleApplication
COMPILER = g++
CFLAGS = -std=c++17 -g3
OS = "Ubuntu" # Mac
ifneq ($(OS), "Ubuntu")
	VULKANDIR = /home/honolulu/programs/downloaded_libraries/vulkanSDK/x86_64
	GLFWDIR = /usr/local/Cellar/glfw/3.3.6
	LDFLAGS = $(VULKANDIR)/lib/libvulkan.so.1.3.204 $(VULKANDIR)/lib/libvulkan.so.1 /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
	INCLUDE = -I/usr/include -I$(VULKANDIR)/include -I./include
	VK_ICD_FILENAMES := $(VULKANDIR)/etc/vulkan/icd.d/MoltenVK_icd.json
	VK_LAYER_PATH := $(VULKANDIR)/etc/vulkan/explicit_layer.d
else # for Mac OS
	VULKANDIR = /Users/toyotariku/VulkanSDK/1.2.198.1/macOS
	GLFWDIR = /usr/local/Cellar/glfw/3.3.6
	LDFLAGS = $(VULKANDIR)/lib/libvulkan.1.2.198.dylib $(VULKANDIR)/lib/libvulkan.1.dylib $(GLFWDIR)/lib/libglfw.3.3.dylib
	INCLUDE = -I/usr/local/include -I$(VULKANDIR)/include -I./include
	VK_ICD_FILENAMES := $(VULKANDIR)/share/vulkan/icd.d/MoltenVK_icd.json
	VK_LAYER_PATH := $(VULKANDIR)/share/vulkan/explicit_layer.d
endif
SOURCEDIR = ./src
SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
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
	-rm -rf $(OBJECTDIR)
	-rm -f $(notdir $(TARGET)) $(DEPENDS)

-include $(DEPENDS)