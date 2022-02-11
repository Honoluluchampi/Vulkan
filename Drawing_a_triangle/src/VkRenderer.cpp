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

void VkRenderer::drawFrame(const VkDeviceManager& deviceManager,
    const std::vector<VkCommandBuffer>& commandBuffers)
{
    // Acquiring an image from the swap chain
    // index of the aquired image (VkImage in swapChainImages array)
    uint32_t imageIndex;
    const auto& device = deviceManager.getDevice();
    const auto& swapChain = deviceManager.getSwapChainRef();
    // specify a timeout in nanoseconds for an image
    auto timeout = UINT64_MAX;
    // after acquiring image, the imageSemaphore is signaled
    vkAcquireNextImageKHR(device, swapChain, timeout,
        imageAvailableSemaphore_m, VK_NULL_HANDLE, &imageIndex);

    //submitting the command buffer
    const auto& graphicsQueue = deviceManager.getGraphicsQueueRef();
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // the index of waitSemaphores corresponds to the index of waitStages
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore_m};
    // which stage of the pipeline to wait
    VkPipelineStageFlags waitStages[] = 
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    // which command buffers to actually submit for execution
    // should submit the command buffer that binds the swap chain image 
    // we just acquired as color attachiment.
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    // specify which semaphores to signal once the comand buffer have finished execution
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore_m};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    // submit the command buffer to the graphics queue
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    // configure subpass dependencies in VkRenderPassFacotry::createRenderPass

    // presentation
    // submit the result back to the swap chain to have it eventually show up on the screen
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    // necessary for multi swap chain
    presentInfo.pResults = nullptr; // optional
    const auto& presentQueue = deviceManager.getPresentQueueRef();
    vkQueuePresentKHR(presentQueue, &presentInfo);
}