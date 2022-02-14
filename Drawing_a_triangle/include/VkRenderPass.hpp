#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeviceManager.hpp>

// tell Vulkan about the framebuffer attachments that will be used while rendering
// how many color and depth buffers there will be
// how many samples to use for each of them
class VkRenderPassFactory
{
public:
    void createRenderPass(const VkDevice& device, 
        const VkFormat& swapChainImageFormat, VkRenderPass* pRenderPass);
private:
    void createAttachmentDescription(const VkFormat& swapChainImageFormat);
    void createSubPass();
    VkSubpassDependency createSubpassDependency();

    VkAttachmentDescription colorAttachment_m{};
    VkAttachmentReference colorAttachmentRef_m{};
    VkSubpassDescription subpass_m{};
};