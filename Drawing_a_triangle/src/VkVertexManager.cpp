#include <VkVertexManager.hpp>
#include <iostream>
#include <cstring>

void VkVertexManager::createVerticesData() 
{
    
};

VkVertexInputBindingDescription VkVertexManager::Vertex::getBindingDescription()
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

void VkVertexManager::createVertexBuffer
    (const VkDevice& device, const VkPhysicalDevice& physicalDevice)
{
    createVertexBufferImpl(device);
    allocateVertexMemory(device, physicalDevice);
    // associate the memory with the buffer
    vkBindBufferMemory(device, vertexBuffer_m, vertexBufferMemory_m, 0);
    // copy the vertex data to the buffer
    fillVertexBuffer(device);
}

void VkVertexManager::createVertexBufferImpl(const VkDevice& device)
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
        throw std::runtime_error("failed to create vertex buffer!");
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, 
    const VkPhysicalDevice& physicalDevice)
{
    // available types of memory
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        // check if the result of the bitwise AND is not just non-zero, 
        // but equal to the desired properties bit field
        if (typeFilter & (i << i) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
            return i;
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void VkVertexManager::allocateVertexMemory
    (const VkDevice& device, const VkPhysicalDevice& physicalDevice)
{
    // memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer_m, &memRequirements);

    // memory allocation
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    // we need two properties of the memory type
    allocInfo.memoryTypeIndex = findMemoryType(
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        physicalDevice);
    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory_m) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");
}

void VkVertexManager::fillVertexBuffer(const VkDevice& device)
{
    // handle of the memory
    void* data;
    vkMapMemory(device, vertexBufferMemory_m, 0, sizeof(vertices_m[0]) * vertices_m.size(), 0, &data);
    std::memcpy(data, vertices_m.data(), sizeof(vertices_m[0]) * vertices_m.size());
    vkUnmapMemory(device, vertexBufferMemory_m);
}

void VkVertexManager::destroyVertexBuffer(const VkDevice& device)
{
    vkDestroyBuffer(device, vertexBuffer_m, nullptr);
    vkFreeMemory(device, vertexBufferMemory_m, nullptr);
}

VkBuffer& VkVertexManager::getVertexBufferRef()
    { return vertexBuffer_m; }

size_t VkVertexManager::getVerticesSize()
    { return vertices_m.size(); }