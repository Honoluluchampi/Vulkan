#include <VkFramebufferFactory.hpp>
#include <iostream>

std::vector<VkFramebuffer>& VkFramebufferFactory::getSwapChainFrameBuffersRef()
{
    return swapChainFramebuffers_m;
}

void VkFramebufferFactory::createFramebuffers
    (const VkDeviceManager& deviceManager, const VkRenderPass& renderPass)
{
    // get refferences
    const auto& device = deviceManager.getDevice();
    const auto& swapChainExtent = deviceManager.getSwapChainExtent();
    const auto& swapChainImageViews = deviceManager.getSwapChainImageViews();
    swapChainFramebuffers_m.resize(swapChainImageViews.size());

    for(size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = { swapChainImageViews[i] };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = 
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        // renderPass the framebuffer needs to be compatible
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;
        // create framebuffer
        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr,
            &swapChainFramebuffers_m[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer!");
    }
}

void VkFramebufferFactory::destroyFramebuffers(const VkDeviceManager& deviceManager)
{
    const auto& device = deviceManager.getDevice();
    for(const auto& framebuffer : swapChainFramebuffers_m) 
        vkDestroyFramebuffer(device, framebuffer, nullptr);
}