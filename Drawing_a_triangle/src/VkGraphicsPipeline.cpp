#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <VkGraphicsPipeline.hpp>
#include <VkRenderPass.hpp>
#include <VkVertexManager.hpp>

const VkRenderPass& VkGraphicsPipelineFactory::getRenderPassRef()
{
    return renderPass_m;
}

const VkPipeline& VkGraphicsPipelineFactory::getGraphicsPipelineRef()
{
    return graphicsPipeline_m;
}

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

void VkGraphicsPipelineFactory::createRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat)
{
    // render pass
    renderPassFactory_m.createRenderPass(device, swapChainImageFormat, &renderPass_m);
}

void VkGraphicsPipelineFactory::createGraphicsPipeline
    (const VkDevice& device, const VkExtent2D& swapChainExtent)
{
    auto vertShaderCode = readFile("./spv/vert.spv");
    auto fragShaderCode = readFile("./spv/frag.spv");
    vertShaderModule_m  = createShaderModule(vertShaderCode, device);
    fragShaderModule_m  = createShaderModule(fragShaderCode, device);
    // assign these modules to a specific pipeline stage
    // auto vertShaderStageInfo    = createVertexShaderStageInfo();
    // auto fragShaderStageInfo    = createFragmentShaderStageInfo();
    VkPipelineShaderStageCreateInfo shaderStages[]
        = {createVertexShaderStageInfo(), createFragmentShaderStageInfo()};
    // fixed functions
    auto vertexInputInfo    =   createVertexInputInfo();
    auto inputAssemblyInfo  =   createInputAssemblyInfo();
    // viewport
    auto viewport   =           createViewport(swapChainExtent);
    auto scissor    =           createScissor(swapChainExtent);
    auto viewportInfo =         createViewportInfo(viewport, scissor);
    auto rasterizer =           createRasterizer();
    auto multisampling =        createMultisampleState();
    auto colorBlendAttachment = createColorBlendingAttachment();
    auto colorBlendState =      createColorBlendingState(colorBlendAttachment);

    // pipeline creation
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // shader stages
    pipelineInfo.stageCount = 2; // vertex and fragments stage
    pipelineInfo.pStages = shaderStages;
    // fixed functions
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = nullptr;
    // pipeline layout
    createPipelineLayout(device);
    pipelineInfo.layout = pipelineLayout_m;
    // pass by copy?
    pipelineInfo.renderPass = renderPass_m;
    pipelineInfo.subpass = 0;
    // its possible to create multiple VkPipeline objects in a single call
    // second parameter means cache objects enables significantly faster creation
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
        &pipelineInfo, nullptr, &graphicsPipeline_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create graphics pipeline!");
    std::cout << "the graphics pipeline has been created!" << std::endl;
    vkDestroyShaderModule(device, vertShaderModule_m, nullptr);
    vkDestroyShaderModule(device, fragShaderModule_m, nullptr);
}

VkShaderModule VkGraphicsPipelineFactory::createShaderModule
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
    VkGraphicsPipelineFactory::createVertexShaderStageInfo()
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule_m;
    // the function to invoke
    vertShaderStageInfo.pName = "main";
    return vertShaderStageInfo;
}

VkPipelineShaderStageCreateInfo
    VkGraphicsPipelineFactory::createFragmentShaderStageInfo()
{
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule_m;
    // the function to invoke
    fragShaderStageInfo.pName = "main";
    return fragShaderStageInfo;
}

VkPipelineVertexInputStateCreateInfo 
    VkGraphicsPipelineFactory::createVertexInputInfo()
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // accept vertex data
    static auto bindingDescription = VkVertexManager::Vertex::getBindingDescription();
    static auto attributeDescriptions = VkVertexManager::Vertex::getAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; //optional
    vertexInputInfo.vertexAttributeDescriptionCount = 
        static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); //optional
    return vertexInputInfo;
}

// what kind of geometry will be drawn from the vertices (topoloby) and
// if primitive restart should be enabled
VkPipelineInputAssemblyStateCreateInfo
    VkGraphicsPipelineFactory::createInputAssemblyInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

// transformation of the image
VkViewport VkGraphicsPipelineFactory::createViewport(const VkExtent2D& extent)
{
    // draw entire framebuffer
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

// cut the region of the framebuffer(swap chain)
VkRect2D VkGraphicsPipelineFactory::createScissor(const VkExtent2D& extent)
{
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent.width = static_cast<float>(extent.width);
    scissor.extent.height = static_cast<float>(extent.height);
    return scissor;
}

VkPipelineViewportStateCreateInfo VkGraphicsPipelineFactory::createViewportInfo
    (const VkViewport& viewport, const VkRect2D& scissor)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // by enabling a GPU feature in logical device creation,
    // its possible to use multiple viewports
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    return viewportState;
}

VkPipelineRasterizationStateCreateInfo
    VkGraphicsPipelineFactory::createRasterizer()
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // using this requires enabling a GPU feature
    rasterizer.depthClampEnable = VK_FALSE;
    // if rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes
    // through the rasterizer stage, basically disables any output to the frame_buffer
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // how fragments are generated for geometry
    // using any mode other than fill requires GPU feature
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // consider this when shadow mapping is necessary
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    return rasterizer;
}

// used for anti-aliasing
VkPipelineMultisampleStateCreateInfo VkGraphicsPipelineFactory::createMultisampleState()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    return multisampling;
}

// color blending for alpha blending
VkPipelineColorBlendAttachmentState VkGraphicsPipelineFactory::createColorBlendingAttachment()
{
    // per framebuffer struct
    // in contrast, VkPipelineColcorBlendStateCreateInfo is global color blending settings
    VkPipelineColorBlendAttachmentState colorBlendAttachment{}; 
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
    VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT; colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; 
    colorBlendAttachment.dstColorBlendFactor =VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; 
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo VkGraphicsPipelineFactory::createColorBlendingState
    (VkPipelineColorBlendAttachmentState& colorBlendingAttachment)
{
    VkPipelineColorBlendStateCreateInfo colorBlending{}; 
    colorBlending.sType =
    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE; 
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional 
    colorBlending.attachmentCount = 1; 
    colorBlending.pAttachments = &colorBlendingAttachment; 
    colorBlending.blendConstants[0] = 0.0f; // Optional 
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional 
    colorBlending.blendConstants[3] = 0.0f; // Optional
    return colorBlending;
}

// a limited amount of the state can be actually be changed without recreating the pipeline
VkPipelineDynamicStateCreateInfo VkGraphicsPipelineFactory::createDynamicState()
{
    VkDynamicState dynamicStates[] = {};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 0; 
    dynamicState.pDynamicStates = nullptr;
    return dynamicState;
}

void VkGraphicsPipelineFactory::createPipelineLayout(const VkDevice& device)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create pipeline layout!");
}

void VkGraphicsPipelineFactory::destroyGraphicsPipeline(const VkDevice& device)
{
    vkDestroyPipeline       (device, graphicsPipeline_m, nullptr);
    vkDestroyPipelineLayout (device, pipelineLayout_m, nullptr);
}

void VkGraphicsPipelineFactory::destroyRenderPass(const VkDevice& device)
{
    vkDestroyRenderPass     (device, renderPass_m, nullptr);
}