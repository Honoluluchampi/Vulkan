#include "VkDeviceManager.hpp"
#include <stdexcept>
#include <iostream>

void VkDeviceManager::deviceCleanup(const VkInstance& instance)
{
    // VkQueue is automatically destroyed when its device is deleted
    vkDestroyDevice(device_m,nullptr);
    // surface destruction
    vkDestroySurfaceKHR(instance, surface_m, nullptr);
}
// get apps VkInstance
void VkDeviceManager::pickPhysicalDevice(VkInstance& instance)
{
    // rate device suitability if its nesessary
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0){
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    // allocate an array to hold all of the VkPhysicalDevice handle
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    physicalDevice_m = VK_NULL_HANDLE;
    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice_m = device;
            break;
        }
    }
    if (physicalDevice_m == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VkDeviceManager::isDeviceSuitable(const VkPhysicalDevice& device) 
{
    // VkPhysicalDeviceProperties deviceProperties; 
    // VkPhysicalDeviceFeatures deviceFeatures; 
    // vkGetPhysicalDeviceProperties(device, &deviceProperties); 
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
    //     && deviceFeatures.geometryShader;
    auto indices = findQueueFamilies(device);
    return indices.isComplete();
}

VkDeviceManager::QueueFamilyIndices VkDeviceManager::findQueueFamilies(const VkPhysicalDevice& device)
{
    VkDeviceManager::QueueFamilyIndices indices;
    // retrieve the list of quque families 
    uint32_t queueFamilyCount = 0;
    // use same function
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    // vkQueueFamilyProperties struct contains some details about the queue family
    // the type of operations, the number of queue that can be created based on that family
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    // check whether at least one queueFamily support VK_QUEUEGRAPHICS_BIT
    int i = 0;
    for (const auto& queueFamily : queueFamilies){
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily_m = i;
            break;
        }
        i++;
    }
    return indices;
}

inline bool VkDeviceManager::QueueFamilyIndices::isComplete()
{
    return graphicsFamily_m.has_value();
}

void VkDeviceManager::createLogicalDevice
    (const bool& enableValidationLayers, const std::vector<const char*>& validationLayers)
{
    auto indices = findQueueFamilies(physicalDevice_m);
    // VkDeviceQueueCreateInfo descrives the number of queues we want for a single queue family
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily_m.value();
    queueCreateInfo.queueCount = 1;
    // Vulkan lets us assign priorities to queues to influence the scheduling of commmand buffer execut9on
    // using floating point numbers between 0.0 and 1.0
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    // we need nothing special right now
    VkPhysicalDeviceFeatures deviceFeatures{};
    // filling in the main VkDeviceCreateInfo structure;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType =  VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    // nothing right now
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    // instantiate the logical device
    // logical devices dont interact directly with  instances 
    if (vkCreateDevice(physicalDevice_m, &createInfo, nullptr, &device_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");
    // retrieve queue handles for each queue family
    // simply use index 0, because were only creating a single queue from  this family
    vkGetDeviceQueue(device_m, indices.graphicsFamily_m.value(), 0, &graphicsQueue_m);
}

void VkDeviceManager::createSurface
    (const VkInstance& instance, GLFWwindow* window)
{
    // glfwCreateWindowSurface is implemented to multi-platfowm
    // we dont have to implement createSurface function using platform-specific extension
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}