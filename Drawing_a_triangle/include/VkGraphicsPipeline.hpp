#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <VkDeviceManager.hpp>

class VkGraphicsPipeline
{
public:
    void createGraphicsPipeline(const VkDeviceManager& upDeviceManager);
private:
    // wrap the shader code in a VkShaderModule object
    VkShaderModule createShaderModule
        (const std::vector<char>& code, const VkDevice& device);
    VkPipelineShaderStageCreateInfo         createVertexShaderStageInfo();
    VkPipelineShaderStageCreateInfo         createFragmentShaderStageInfo();
    VkPipelineVertexInputStateCreateInfo    createVertexInputInfo();
    VkPipelineInputAssemblyStateCreateInfo  createInputAssemblyInfo();
    VkViewport  createViewport(const VkExtent2D& extent);
    VkRect2D    createScissor(const VkExtent2D& extent);
    VkPipelineViewportStateCreateInfo createViewportInfo
        (const VkViewport& viewport, const VkRect2D& scissor);

    std::unique_ptr<VkDeviceManager> upDeviceManager_m;    
    VkShaderModule vertShaderModule_m;
    VkShaderModule fragShaderModule_m;
};