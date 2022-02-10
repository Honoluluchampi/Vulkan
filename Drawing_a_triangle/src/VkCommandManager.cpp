#include <VkCommandManager.hpp>
#include <iostream>

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

void VkCommandManager::createCommandBuffers()
{

}

void VkCommandManager::destroyCommandPoolandBuffers(const VkDevice& device)
{
    vkDestroyCommandPool(device, commandPool_m, nullptr);
}