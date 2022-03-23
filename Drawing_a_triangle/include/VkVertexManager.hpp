#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <VkDeviceManager.hpp>

class VkVertexManager
{
public:
    struct Vertex
    {
        glm::vec2 position_m;
        glm::vec3 color_m;
        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2>
            getAttributeDescriptions();
    };
    void createVerticesData();
    void createVertexBuffer(const VkDeviceManager& deviceManager, VkCommandPool& commandPool);
    void destroyVertexBuffer(const VkDevice& device);
    VkBuffer& getVertexBufferRef();
    size_t getVerticesSize();
private:
    void createBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, VkDeviceSize size, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void fillVertexBuffer(const VkDevice& device);
    void copyBuffer(const VkDevice& device, VkCommandPool& commandPool, const VkQueue& graphicsQueue, 
        VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
    // use exactly the same position and color values as the shader file
    std::vector<Vertex> vertices_m;
    // handle of the vertex buffer
    VkBuffer vertexBuffer_m;
    VkDeviceMemory vertexBufferMemory_m;    
};