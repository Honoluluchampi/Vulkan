#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

class VkVertexFactory
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
private:
    // use exactly the same position and color values as the shader file
    std::vector<Vertex> vertices_m;
};