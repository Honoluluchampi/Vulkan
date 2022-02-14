#include <VkDeviceManager.hpp>
#include <stdexcept>
#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>

const VkDevice& VkDeviceManager::getDevice() const
    {return device_m;}

const VkPhysicalDevice& VkDeviceManager::getPhysicalDevice() const
    {return physicalDevice_m;}

const VkQueue& VkDeviceManager::getGraphicsQueueRef() const 
    {return graphicsQueue_m;}

const VkQueue& VkDeviceManager::getPresentQueueRef() const
    {return presentQueue_m;}

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
// ensure there is at least one available physical device and
// the debice can present images to the surface we created
bool VkDeviceManager::isDeviceSuitable(const VkPhysicalDevice& device) 
{
    // VkPhysicalDeviceProperties deviceProperties; 
    // VkPhysicalDeviceFeatures deviceFeatures; 
    // vkGetPhysicalDeviceProperties(device, &deviceProperties); 
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
    //     && deviceFeatures.geometryShader;
    auto indices = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        auto swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats_m.empty() && 
            !swapChainSupport.presentModes_m.empty();
    }
    return indices.isComplete() && extensionsSupported 
        && swapChainAdequate;
}

// check for swap chain extension
bool VkDeviceManager::checkDeviceExtensionSupport(const VkPhysicalDevice& device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr,
        &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, 
        &extensionCount, availableExtensions.data());
    std::set<std::string> requiredExtensions(deviceExtensions_m.begin(), deviceExtensions_m.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    return requiredExtensions.empty();
}

VkDeviceManager::SwapChainSupportDetails VkDeviceManager::querySwapChainSupport(const VkPhysicalDevice& device)
{
    SwapChainSupportDetails details;
    // surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_m, &details.capabilities_m);
    // surface format list
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_m, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats_m.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_m, &formatCount, details.formats_m.data());
    }
    // presentation mode list
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_m, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes_m.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_m, &presentModeCount, details.presentModes_m.data());
    }
    return details;
}

VkDeviceManager::QueueFamilyIndices VkDeviceManager::findQueueFamilies
    (const VkPhysicalDevice& device)
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
    VkBool32 presentSupport = false;
    for (const auto& queueFamily : queueFamilies){
        // same i for presentFamily and graphicsFamily improves the performance
        // vulkan: No DRI3 support detected - required for presentation
        // Note: you can probably enable DRI3 in your Xorg config
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_m, &presentSupport);
        if (presentSupport) indices.presentFamily_m = i;
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily_m = i;
            if(presentSupport) break;
        }
        i++;
    }
    return indices;
}

inline bool VkDeviceManager::QueueFamilyIndices::isComplete()
{
    return graphicsFamily_m.has_value() && presentFamily_m.has_value();
}

void VkDeviceManager::createLogicalDevice
    (const bool& enableValidationLayers, const std::vector<const char*>& validationLayers)
{
    auto indices = findQueueFamilies(physicalDevice_m);
    // create a set of all unique queue famililes that are necessary for required queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    // if queue families are the same, handle for those queues are also same
    std::set<uint32_t> uniqueQueueFamilies = 
    {indices.graphicsFamily_m.value(), indices.presentFamily_m.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        // VkDeviceQueueCreateInfo descrives the number of queues we want for a single queue family
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = 
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        // Vulkan lets us assign priorities to queues to influence the scheduling of commmand buffer execut9on
        // using floating point numbers between 0.0 and 1.0
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    // we need nothing special right now
    VkPhysicalDeviceFeatures deviceFeatures{};
    // filling in the main VkDeviceCreateInfo structure;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType =  VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    // nothing right now
    createInfo.pEnabledFeatures = &deviceFeatures;
    // enable device extension 
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions_m.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions_m.data();
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
    vkGetDeviceQueue(device_m, indices.presentFamily_m.value(), 0, &presentQueue_m);
}

void VkDeviceManager::createSurface
    (const VkInstance& instance, GLFWwindow* window)
{
    // glfwCreateWindowSurface is implemented to multi-platfowm
    // we dont have to implement createSurface function using platform-specific extension
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}