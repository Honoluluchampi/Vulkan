#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class VkSwapChainManager
{
public: 
    VkSwapChainManager(const class VkDeviceManager& dm, GLFWwindow* wdw)
        : deviceManagerRef_m(const_cast<VkDeviceManager&>(dm)), windowRef_m(wdw){}
    void createSwapChain(const uint32_t width, const uint32_t height);
    void createImageViews();
    void destroyImageViews();
    void destroySwapChain();

    // getter
    const VkExtent2D& getSwapChainExtentRef() const;
    const VkFormat& getSwapChainImageFormatRef() const;
    const std::vector<VkImageView>& getSwapChainImageViewsRef() const;
    const VkSwapchainKHR& getSwapChainRef() const;
    const size_t getSwapChainImagesNum() const;
private:
    // choosing the right settings for the swap chain
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const
        std::vector<VkSurfaceFormatKHR>& availableFormats);
    // most important settings for swap chain
    VkPresentModeKHR chooseSwapPresentMode(const
        std::vector<VkPresentModeKHR>& availablePresentModes);
    // choose resolution of output
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    class VkDeviceManager& deviceManagerRef_m;
    GLFWwindow* windowRef_m;
    VkSwapchainKHR swapChain_m;
    // handles of VkImages in a swap chain
    std::vector<VkImage> swapChainImages_m;
    VkFormat swapChainImageFormat_m;
    VkExtent2D swapChainExtent_m;
    std::vector<VkImageView> swapChainImageViews_m;
};