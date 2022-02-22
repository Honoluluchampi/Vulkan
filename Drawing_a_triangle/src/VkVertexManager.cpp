#include <VkVertexManager.hpp>
#include <iostream>

void VkVertexManager::createVerticesData() 
{
    vertices_m =
    {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 0.0f, 1.0f}}
    };
};

VkVertexInputBindingDescription VkVertexManager::Vertex::getBindingDescription()
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
    VkVertexManager::Vertex::getAttributeDescriptions()
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

void VkVertexManager::createVertexBuffer(const VkDevice& device)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // specify the size of the vertex buffer
    bufferInfo.size = sizeof(vertices_m[0]) * vertices_m.size();
    // specify the usage of the buffer
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    // buffers can be owned by a specific queue family or be shared 
    // between multiple at the same time.
    // vertex buffers are only used from the graphics queue
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer_m) != VK_SUCCESS)
        std::runtime_error("failed to create vertex buffer!");
}

void VkVertexManager::destroyVertexBuffer(const VkDevice& device)
{
    vkDestroyBuffer(device, vertexBuffer_m, nullptr);
}