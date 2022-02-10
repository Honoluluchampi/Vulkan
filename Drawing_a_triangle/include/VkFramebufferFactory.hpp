#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <VkDeviceManager.hpp>

class VkFramebufferFactory
{
public:
    VkFramebufferFactory(){}
    void createFramebuffers
        (const VkDeviceManager& deviceManager, const VkRenderPass& renderPass);
    void destroyFramebuffers(const VkDeviceManager& deviceManager);
    std::vector<VkFramebuffer>& getSwapChainFrameBuffersRef();
private:
    std::vector<VkFramebuffer> swapChainFramebuffers_m;
};