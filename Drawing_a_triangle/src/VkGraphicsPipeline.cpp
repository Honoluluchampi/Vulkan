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
    vertShaderModule_m = createShaderModule(vertShaderCode, device);
    fragShaderModule_m = createShaderModule(fragShaderCode, device);
    // assign these modules to a specific pipeline stage
    auto shaderStageInfo = createPipelineShaderStageInfo();
    auto vertexInputInfo = createVertexInputInfo();
    
    vkDestroyShaderModule(device, vertShaderModule_m, nullptr);
    vkDestroyShaderModule(device, fragShaderModule_m, nullptr);
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

VkPipelineShaderStageCreateInfo 
    VkGraphicsPipeline::createPipelineShaderStageInfo()
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule_m;
    // the function to invoke
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule_m;
    // the function to invoke
    fragShaderStageInfo.pName = "main";
}

VkPipelineVertexInputStateCreateInfo VkGraphicsPipeline::createVertexInputInfo()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; //optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; //optional
    return vertexInputInfo;
}