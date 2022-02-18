#include <VkCommandManager.hpp>
#include <iostream>

const std::vector<VkCommandBuffer>& VkCommandManager::getCommandBufferRef() const
    {return commandBuffers_m;}
// Command pools manage the memory that is used to store the buffers 
// and com- mand buffers are allocated from them.
void VkCommandManager::createCommandPool(const VkDeviceManager& deviceManager)
{
    // make sure this const_cast doesnt cause any error.
    auto queueFamilyIndices = 
        const_cast<VkDeviceManager&>(deviceManager).findQueueFamilies(deviceManager.getPhysicalDevice());
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily_m.value();
    poolInfo.flags = 0; // optional
    if (vkCreateCommandPool(deviceManager.getDevice(), &poolInfo, nullptr,
        &commandPool_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
}

void VkCommandManager::createCommandBuffers
    (const VkDevice& device, const VkRenderPass& renderPass, const std::vector<VkFramebuffer>& swapChainFramebuffers,
    const VkPipeline& graphicsPipeline, const VkExtent2D& swapChainExtent)
{
    commandBuffers_m.resize(swapChainFramebuffers.size());
    // specify command pool and number of buffers to allocate
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_m;
    // if the allocated command buffers are primary or secondary command buffers
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_m.size());
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers_m.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
    // execute command buffer-relevant functions
    executeCommandFunctions(renderPass, swapChainExtent, swapChainFramebuffers, graphicsPipeline);
}

void VkCommandManager::destroyCommandPool(const VkDevice& device)
{
    vkDestroyCommandPool(device, commandPool_m, nullptr);
}

void VkCommandManager::destroyCommandBuffers(const VkDevice& device)
{
    // nothing to do
}

void VkCommandManager::executeCommandFunctions
    (const VkRenderPass& renderPass, const VkExtent2D& swapChainExtent,
     const std::vector<VkFramebuffer>& swapChainFramebuffers, const VkPipeline& graphicsPipeline)
{
    for (size_t i = 0; i < commandBuffers_m.size(); i++) {
        // start reconding command buffers
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // how to use the command buffer
        beginInfo.flags = 0;
        // state to inherit from the calling primary command buffers
        // (only relevant for secondary command buffers)
        beginInfo.pInheritanceInfo = nullptr;
        if (vkBeginCommandBuffer(commandBuffers_m[i], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");
        
        // starting a render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        // the pixels outside this region will have undefined values
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        // last parameter controls how the drawing commands within the render pass
        // will be provided. 
        vkCmdBeginRenderPass(commandBuffers_m[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // basic drawing commands
        // bind the graphics pipeline
        // the second parameter specifies if the pipeline object is a graphics or compute pipeline
        vkCmdBindPipeline(commandBuffers_m[i], 
            VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        // draw triangle
        // vertexCount, instanceCount, firstVertex, firstInstance
        vkCmdDraw(commandBuffers_m[i], 3, 1, 0, 0);

        // finish render pass and recording the comand buffer
        vkCmdEndRenderPass(commandBuffers_m[i]);
        if (vkEndCommandBuffer(commandBuffers_m[i]) != VK_SUCCESS) 
            throw std::runtime_error("failed to record command buffer!");
    }
}