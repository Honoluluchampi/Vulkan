#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VkGraphicsPipeline
{
public:
    void createGraphicsPipeline(const VkDevice& device);
    // wrap the shader code in a VkShaderModule object
    VkShaderModule createShaderModule
        (const std::vector<char>& code, const VkDevice& device);
    void createPipelineShaderStage();
private:
    VkShaderModule vertShaderModule_m;
    VkShaderModule fragShaderModule_m;
};