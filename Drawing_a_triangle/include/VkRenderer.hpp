#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeviceManager.hpp>

class VkRenderer
{
public:
    void createSyncObjects(const VkDevice& device, size_t imagesNum);
    void destroyRenderer(const VkDevice& device);
    bool drawFrame(const VkDeviceManager& deviceManager, const VkSwapchainKHR& swapChain,
        const std::vector<VkCommandBuffer>& commandBuffer);

    // handle resizes explicitly
    bool framebufferResized = false;
private:
    // an image has been acquired and is ready for rendering
    std::vector<VkSemaphore> imageAvailableSemaphores_m;
    // rendering has finished and presentation can happen
    std::vector<VkSemaphore> renderFinishedSemaphores_m;
    // to use the right pair of semaphores every time
    size_t currentFrame_m = 0;
    // for CPU-GPU synchronization
    std::vector<VkFence> inFlightFences_m;
    // wait on before a new frame can use that image
    std::vector<VkFence> imagesInFlight_m;
};