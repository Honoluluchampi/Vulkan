#include <VkRenderer.hpp>
#include <iostream>

// how many frames should be processed concurrently
constexpr size_t MAX_FRAMES_IN_FIGHT = 2;

void VkRenderer::createSyncObjects(const VkDeviceManager& deviceManager)
{
    const auto& device = deviceManager.getDevice();
    auto imagesNum = deviceManager.getSwapChainImagesNum();

    imageAvailableSemaphores_m.resize(MAX_FRAMES_IN_FIGHT);
    renderFinishedSemaphores_m.resize(MAX_FRAMES_IN_FIGHT);
    inFlightFences_m.resize(MAX_FRAMES_IN_FIGHT);
    // initially not a single framce is using an image, so initialize it to no fence
    imagesInFlight_m.resize(imagesNum, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // initialize fences in the signaled state as if they had been rendered an initial frame
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
 
    for (size_t i = 0; i < MAX_FRAMES_IN_FIGHT; i++) {
        // future version of the vulkan api may add functionality for other parameters
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores_m[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores_m[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences_m[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create semaphores!");
    }
}

void VkRenderer::destroyRenderer(const VkDevice& device)
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores_m[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores_m[i], nullptr);
        vkDestroyFence(device, inFlightFences_m[i], nullptr);
    }
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

    // wait for the frame to be finished
    vkWaitForFences(device, 1, &inFlightFences_m[currentFrame_m], VK_TRUE, timeout);

    // after acquiring image, the imageSemaphore is signaled
    vkAcquireNextImageKHR(device, swapChain, timeout,
        imageAvailableSemaphores_m[currentFrame_m], VK_NULL_HANDLE, &imageIndex);
    // check if a previous frame is using this image
    if(imagesInFlight_m[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(device, 1, &imagesInFlight_m[imageIndex], VK_TRUE, timeout);
    // mark the image as now being in use by this frame
    imagesInFlight_m[imageIndex] = inFlightFences_m[currentFrame_m];

    //submitting the command buffer
    const auto& graphicsQueue = deviceManager.getGraphicsQueueRef();
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // the index of waitSemaphores corresponds to the index of waitStages
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_m[currentFrame_m]};
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
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_m[currentFrame_m]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // need to be manually restore the fence to the unsignaled state
    vkResetFences(device, 1, &inFlightFences_m[currentFrame_m]);

    // submit the command buffer to the graphics queue with fence
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences_m[currentFrame_m]) != VK_SUCCESS)
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

    currentFrame_m = (currentFrame_m + 1) % MAX_FRAMES_IN_FIGHT;
}