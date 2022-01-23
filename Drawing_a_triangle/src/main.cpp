#define GLFW_INCLUDE_VULKAN // by putting this, vulkan.h is included in glfw3.h
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h> // functions, structures, enumerations from LunarG SDK
#include <iostream>     
#include <stdexcept> // report and propagate an error
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>
#include <vector>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// determine wheather enable validation layers or not by using NDEBUG
#ifdef NDEBUG  // not debug
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:
    void initWindow()// init GLFW, create window
    {
        glfwInit();
        // disable openGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // create actual window
        // 4th arg : choosing monitor, 5th : only relevant to openGL
        window_m = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void initVulkan()
    {
        createInstance();
    }
    void mainLoop()
    {
        while(!glfwWindowShouldClose(window_m)){
            glfwPollEvents();
        }
    }
    void cleanup()
    {
        vkDestroyInstance(instance_m, nullptr);
        // once window_m is closed, destroy resources and terminate glfw
        glfwDestroyWindow(window_m);
        glfwTerminate();
    }

    void checkingForExtensionSupport()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        // each VkExtensionProperties contains the name and version of an extension
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::cout << "available extensions: \n";
        for (const auto &extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
        // check wheather all glfwExtensions are supported
    }
    // same as chckingForExtensionSupport()
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        // check if all of the layers in validationLayers exist in the availableLyaers
        for (const char *layerName : validationLayers){
            bool  layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                return false;
            }
        }
        return true;
    }
    // fill in a struct with some informattion about the application
    void createInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        checkingForExtensionSupport();
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        // validation layers
        if (enableValidationLayers && checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else createInfo.enabledLayerCount = 0;
        std::cout << createInfo.enabledLayerCount << std::endl;
        // check std::cout << glfwExtensionCount << std::endl;
        // 1st : pointer to struct with creation info
        // 2nd : pointer to custom allocator callbacks
        // 3rd : pointer to the variable that stores the handle to the new object
        if (vkCreateInstance(&createInfo, nullptr, &instance_m) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");
    }

    GLFWwindow* window_m;
    VkInstance instance_m;
};

int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}