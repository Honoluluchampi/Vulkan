#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeviceManager.hpp>
#include <vector>

class VkCommandManager
{
public:
    VkCommandManager(){}
    // Command pools manage the memory that is used to store the buffers 
    // and com- mand buffers are allocated from them.
    void createCommandPool(const VkDeviceManager& deviceManager);
    void createCommandBuffers(const VkDevice& device, const VkRenderPass& renderPass, const std::vector<VkFramebuffer>& swapChainFramebuffers, 
        const VkPipeline& graphicsPipeline, const VkExtent2D& swapChainExtent, const VkBuffer& vertexBuffer, size_t verticesCount);
    void destroyCommandPool(const VkDevice& device);
    void destroyCommandBuffers(const VkDevice& device);
    const std::vector<VkCommandBuffer>& getCommandBufferRef() const;
    VkCommandPool& getCommandPoolRef();
private:
    void executeCommandFunctions(const VkRenderPass& renderPass, const VkExtent2D& swapChainExtent, 
        const std::vector<VkFramebuffer>& swapChainFramebuffers, const VkPipeline& graphicsPipeline,
        const VkBuffer& vertexBuffer, size_t verticesCount);
    VkCommandPool commandPool_m;
    std::vector<VkCommandBuffer> commandBuffers_m;
};