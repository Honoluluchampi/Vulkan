#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeviceManager.hpp>

class VkRenderer
{
public:
    void createSemaphores(const VkDevice& device);
    void destroyRenderer(const VkDevice& device);
    void drawFrame(const VkDeviceManager& deviceManager, const std::vector<VkCommandBuffer>& commandBuffer);
private:
    // an image has been acquired and is ready for rendering
    VkSemaphore imageAvailableSemaphore_m;
    // rendering has finished and presentation can happen
    VkSemaphore renderFinishedSemaphore_m;
};