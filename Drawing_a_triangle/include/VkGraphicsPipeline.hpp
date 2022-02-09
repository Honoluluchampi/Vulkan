#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <VkDeviceManager.hpp>

class VkGraphicsPipelineFactory
{
public:
    VkGraphicsPipelineFactory(const VkDeviceManager& deviceManager);
    void createGraphicsPipeline();
    void destroyGraphicsPipeline();
private:
    // wrap the shader code in a VkShaderModule object
    VkShaderModule createShaderModule
        (const std::vector<char>& code, const VkDevice& device);
    VkPipelineShaderStageCreateInfo         createVertexShaderStageInfo();
    VkPipelineShaderStageCreateInfo         createFragmentShaderStageInfo();
    VkPipelineVertexInputStateCreateInfo    createVertexInputInfo();
    VkPipelineInputAssemblyStateCreateInfo  createInputAssemblyInfo();
    // viewport and scissor
    VkViewport  createViewport(const VkExtent2D& extent);
    VkRect2D    createScissor(const VkExtent2D& extent);
    VkPipelineViewportStateCreateInfo createViewportInfo
        (const VkViewport& viewport, const VkRect2D& scissor);
    // rasterizer
    VkPipelineRasterizationStateCreateInfo createRasterizer();
    // multisampling used for anti-aliasing
    VkPipelineMultisampleStateCreateInfo createMultisampleState();
    // color blending for alpha blending
    VkPipelineColorBlendAttachmentState createColorBlendingAttachment();
    VkPipelineColorBlendStateCreateInfo createColorBlendingState
        (VkPipelineColorBlendAttachmentState& colorBlendingAttachment);
    // dynamic state
    VkPipelineDynamicStateCreateInfo createDynamicState();
    // create uniform values (globals that can be changed at drawing time 
    // to alter the behavior of the shaders)
    void createPipelineLayout();
    
    const VkDeviceManager& deviceManager_m;
    VkShaderModule vertShaderModule_m;
    VkShaderModule fragShaderModule_m;
    VkPipelineLayout pipelineLayout_m;
    VkRenderPass renderPass_m;
    VkPipeline graphicsPipeline_m;
};