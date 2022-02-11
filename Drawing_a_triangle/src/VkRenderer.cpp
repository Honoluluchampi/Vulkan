#include <VkRenderer.hpp>
#include <iostream>

void VkRenderer::createSemaphores(const VkDevice& device)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // future version of the vulkan api may add functionality for other parameters
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore_m) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create semaphores!");
}

void VkRenderer::destroyRenderer(const VkDevice& device)
{
    vkDestroySemaphore(device, renderFinishedSemaphore_m, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore_m, nullptr);
}