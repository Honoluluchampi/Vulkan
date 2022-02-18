#include <VKSwapChainManager.hpp>
#include <VkDeviceManager.hpp>
#include <iostream>

const VkExtent2D& VkSwapChainManager::getSwapChainExtentRef() const
    { return swapChainExtent_m; }
const VkFormat& VkSwapChainManager::getSwapChainImageFormatRef() const
    { return swapChainImageFormat_m; }
const std::vector<VkImageView>& VkSwapChainManager::getSwapChainImageViewsRef() const
    { return swapChainImageViews_m; }
const VkSwapchainKHR& VkSwapChainManager::getSwapChainRef() const
    { return swapChain_m; }
const size_t VkSwapChainManager::getSwapChainImagesNum() const
    { return swapChainImages_m.size(); }

void VkSwapChainManager::createSwapChain(const uint32_t width, const uint32_t height)
{
    // dangerous cast
    auto swapChainSupport = deviceManagerRef_m.querySwapChainSupport(deviceManagerRef_m.physicalDevice_m);
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
    createInfo.surface = deviceManagerRef_m.surface_m;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    // the amount of layers each image consists of
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // specify how to handle swap chain images by multiple queue families
    auto indices = deviceManagerRef_m.findQueueFamilies(deviceManagerRef_m.physicalDevice_m);
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
    if (vkCreateSwapchainKHR(deviceManagerRef_m.device_m, &createInfo, nullptr, &swapChain_m) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    // retrieving the handles of images in the swap chain
    vkGetSwapchainImagesKHR(deviceManagerRef_m.device_m, swapChain_m, &imageCount, nullptr);
    swapChainImages_m.resize(imageCount);
    vkGetSwapchainImagesKHR(deviceManagerRef_m.device_m, swapChain_m, &imageCount, swapChainImages_m.data());
    swapChainImageFormat_m = surfaceFormat.format;
    swapChainExtent_m = extent;
}

void VkSwapChainManager::createImageViews()
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
        if (vkCreateImageView(deviceManagerRef_m.getDevice(), &createInfo, nullptr,
            &swapChainImageViews_m[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VkSwapChainManager::destroySwapChain()
{
    for (auto& imageView : swapChainImageViews_m) {
        vkDestroyImageView(deviceManagerRef_m.getDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(deviceManagerRef_m.getDevice(), swapChain_m, nullptr);
}

VkSurfaceFormatKHR VkSwapChainManager::chooseSwapSurfaceFormat
    (const std::vector<VkSurfaceFormatKHR>& availableFormats)
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

VkPresentModeKHR VkSwapChainManager::chooseSwapPresentMode
    (const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        // prefer triple buffering
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkSwapChainManager::chooseSwapExtent
    (const VkSurfaceCapabilitiesKHR& capabilities)
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