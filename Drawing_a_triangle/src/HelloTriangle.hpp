#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangleApplication
{
public:
    HelloTriangleApplication();
    void  run();
private:
    // functions
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void checkingForExtensionSupport();
    bool checkValidationLayerSupport();
    void createInstance();
    // variables
    GLFWwindow* window_m;
    VkInstance instance_m;
    std::vector<const char*> validationLayers_m;
    bool enableValidationLayers_m;
};