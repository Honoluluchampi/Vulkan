#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <VkDeviceManager.hpp>
#include <VKSwapChainManager.hpp>

class VkFramebufferFactory
{
public:
    VkFramebufferFactory(){}
    void createFramebuffers(const VkDevice& device, 
        const VkSwapChainManager& swapChainManager, const VkRenderPass& renderPass);
    void destroyFramebuffers(const VkDeviceManager& deviceManager);
    std::vector<VkFramebuffer>& getSwapChainFrameBuffersRef();
private:
    std::vector<VkFramebuffer> swapChainFramebuffers_m;
};