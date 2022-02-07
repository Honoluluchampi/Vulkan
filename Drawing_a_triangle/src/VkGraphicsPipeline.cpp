#include <fstream>
#include <string>
#include <vector>
#include "VkGraphicsPipeline.hpp"

static std::vector<char> readFile(const std::string& filename)
{
    // ate : start reading at the end of the file
    // binary : read  the file as binary file
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void VkGraphicsPipeline::createGraphicsPipeline(const VkDevice& device)
{
    auto vertShaderCode = readFile("./spv/vert.spv");
    auto fragShaderCode = readFile("./spv/frag.spv");
    auto vertShaderModule = createShaderModule(vertShaderCode, device);
    auto fragShaderModule = createShaderModule(fragShaderCode, device);

    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

VkShaderModule VkGraphicsPipeline::createShaderModule
    (const std::vector<char>& code, const VkDevice& device)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    // char to uint32_t
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");
    return shaderModule;
}