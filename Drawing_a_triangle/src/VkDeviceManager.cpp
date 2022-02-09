#include <VkDeviceManager.hpp>
#include <stdexcept>
#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>

const VkDevice& VkDeviceManager::getDevice() const
{
    return device_m;
}

const VkExtent2D& VkDeviceManager::getSwapChainExtent() const
{
    return swapChainExtent_m;
}

const VkFormat& VkDeviceManager::getSwapChainImageFormat() const 
{
    return swapChainImageFormat_m;
}

void VkDeviceManager::deviceCleanup(const VkInstance& instance)
{
    for (auto& imageView : swapChainImageViews_m) {
        vkDestroyImageView(device_m, imageView, nullptr);
    }
    // destroy swap chain before a logical device
    vkDestroySwapchainKHR(device_m, swapChain_m, nullptr);
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

VkSurfaceFormatKHR VkDeviceManager::chooseSwapSurfaceFormat(const
    std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            std::cout << "surface format : best format detected" << std::endl;
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VkDeviceManager::chooseSwapPresentMode(const
    std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        // prefer triple buffering
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkDeviceManager::chooseSwapExtent(const
    VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = swapChainExtent_m;
        actualExtent.width = 
            std::max(capabilities.minImageExtent.width,
            std::min(capabilities.maxImageExtent.width,
            actualExtent.width));
        actualExtent.height = 
            std::max(capabilities.minImageExtent.height,
            std::min(capabilities.maxImageExtent.height,
            actualExtent.height));
        return actualExtent;
    }
}

void VkDeviceManager::createSwapChain()
{
    auto swapChainSupport = querySwapChainSupport(physicalDevice_m);
    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats_m);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes_m);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities_m);
    // how many images id like to have in the swap chain
    uint32_t imageCount = swapChainSupport.capabilities_m.minImageCount + 1;
    // make sure to not exceed the maximum number of images
    if (swapChainSupport.capabilities_m.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities_m.maxImageCount) {
            imageCount = swapChainSupport.capabilities_m.maxImageCount;
    }
    // fill in a structure with required information
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_m;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    // the amount of layers each image consists of
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // specify how to handle swap chain images by multiple queue families
    auto indices = findQueueFamilies(physicalDevice_m);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily_m.value(),
        indices.presentFamily_m.value()};
    if (indices.graphicsFamily_m != indices.presentFamily_m) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        // this option offers best performance
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // optional
        createInfo.pQueueFamilyIndices = nullptr;
    }
    // like a 90 degree clockwise rotation or horizontal flip
    createInfo.preTransform = swapChainSupport.capabilities_m.currentTransform;
    // ignore alpha channel
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    // ignore the color of obscured pixels
    createInfo.clipped = VK_TRUE;
    // invalid or unoptimized swap chain should be reacreated from scratch
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    // create swap chain
    if (vkCreateSwapchainKHR(device_m, &createInfo, nullptr, &swapChain_m) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    // retrieving the handles of images in the swap chain
    vkGetSwapchainImagesKHR(device_m, swapChain_m, &imageCount, nullptr);
    swapChainImages_m.resize(imageCount);
    vkGetSwapchainImagesKHR(device_m, swapChain_m, &imageCount, swapChainImages_m.data());
    swapChainImageFormat_m = surfaceFormat.format;
    swapChainExtent_m = extent;
}

void VkDeviceManager::createImageViews()
{
    // create image view for all VkImage in the swap chain
    swapChainImageViews_m.resize(swapChainImages_m.size());
    for (size_t i = 0; i < swapChainImages_m.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages_m[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat_m;
        // swizzle the color channeld around
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // subresourceRange describes what images purpose is 
        // and which part of the image should be accessed
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(device_m, &createInfo, nullptr,
            &swapChainImageViews_m[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}