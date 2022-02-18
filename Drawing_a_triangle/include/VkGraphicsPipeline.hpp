#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <VkDeviceManager.hpp>
#include <VkRenderPass.hpp>

class VkGraphicsPipelineFactory
{
public:
    void createGraphicsPipeline(const VkDevice& device, const VkExtent2D& swapChainExtent);
    void createRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat);
    void destroyGraphicsPipeline(const VkDevice& device);
    void destroyRenderPass(const VkDevice& device);
    const VkRenderPass& getRenderPassRef();
    const VkPipeline& getGraphicsPipelineRef();
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
    void createPipelineLayout(const VkDevice& device);
    
    VkShaderModule vertShaderModule_m;
    VkShaderModule fragShaderModule_m;
    VkPipelineLayout pipelineLayout_m;
    // to save the attachments and subpasses refrence
    VkRenderPassFactory renderPassFactory_m;
    VkRenderPass renderPass_m;
    VkPipeline graphicsPipeline_m;
};