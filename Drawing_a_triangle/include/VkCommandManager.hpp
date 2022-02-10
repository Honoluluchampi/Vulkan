#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkDeviceManager.hpp>
#include <vector>

class VkCommandManager
{
public:
    // Command pools manage the memory that is used to store the buffers 
    // and com- mand buffers are allocated from them.
    void createCommandPool(const VkDeviceManager& deviceManager);
    void createCommandBuffers();
    void destroyCommandPoolandBuffers(const VkDevice& device);
private:
    VkCommandPool commandPool_m;
    std::vector<VkCommandBuffer> commandBuffers;
};