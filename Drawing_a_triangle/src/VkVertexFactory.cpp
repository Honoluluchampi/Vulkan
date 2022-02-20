#include <VkVertexFactory.hpp>

void VkVertexFactory::createVerticesData() 
{
    vertices_m =
    {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
    };
};

VkVertexInputBindingDescription VkVertexFactory::Vertex::getBindingDescription()
{
    {
        VkVertexInputBindingDescription bindingDescription{};
        // per-vertex data is packed together in one array, so the index of the 
        // binding in the array is always 0
        bindingDescription.binding   = 0;
        // number of bytes from one entry to the next
        bindingDescription.stride    = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
}

std::array<VkVertexInputAttributeDescription, 2> 
    VkVertexFactory::Vertex::getAttributeDescriptions()
{
    // how to extract a vertex attribute from a chunk of vertex data
    std::array<VkVertexInputAttributeDescription,2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    // the location directive of the input in the vertex shader.
    attributeDescriptions[0].location = 0;
    // same enumeration as color formats
    // 2-component vector of 32-bit float
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    // the offset parameter specifies the number of bytes since the start of the per-vertex data to read from
    // the binding loads one Vertex at a time and the position attribute is at an offset of 0 bytes
    // Vertex::glm::vec2 position_m
    attributeDescriptions[0].offset = offsetof(Vertex, position_m);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color_m);
    return attributeDescriptions;
}