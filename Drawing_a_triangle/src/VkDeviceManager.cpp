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

bool VkDeviceManager::isDeviceSuitable(const VkPhysicalDevice& device) {
    // VkPhysicalDeviceProperties deviceProperties; 
    // VkPhysicalDeviceFeatures deviceFeatures; 
    // vkGetPhysicalDeviceProperties(device, &deviceProperties); 
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
    //     && deviceFeatures.geometryShader;
    return true;
}