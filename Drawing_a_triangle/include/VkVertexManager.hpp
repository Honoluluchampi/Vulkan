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
    void createVertexBuffer(const VkDevice& device);
    void destroyVertexBuffer(const VkDevice& device);
private:
    // use exactly the same position and color values as the shader file
    std::vector<Vertex> vertices_m;
    // handle of the vertex buffer
    VkBuffer vertexBuffer_m;
    VkMemoryRequirements memRequirements_m;
    
};