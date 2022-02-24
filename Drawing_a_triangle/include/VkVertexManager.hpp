#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

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
    void createVertexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice);
    void destroyVertexBuffer(const VkDevice& device);
    VkBuffer& getVertexBufferRef();
    size_t getVerticesSize();
private:
    void createVertexBufferImpl(const VkDevice& device);
    void allocateVertexMemory(const VkDevice& device, const VkPhysicalDevice& physicalDevice);
    void fillVertexBuffer(const VkDevice& device);
    // use exactly the same position and color values as the shader file
    std::vector<Vertex> vertices_m;
    // handle of the vertex buffer
    VkBuffer vertexBuffer_m;
    VkDeviceMemory vertexBufferMemory_m;    
};