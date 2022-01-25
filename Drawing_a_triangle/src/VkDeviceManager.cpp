#include "VkDeviceManager.hpp"
#include <stdexcept>
#include <vector>

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
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.isComplete();
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

void VkDeviceManager::createLogicalDevice()
{
    
}