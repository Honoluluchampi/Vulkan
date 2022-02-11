#include <VkRenderPass.hpp>
#include <iostream>

void VkRenderPassFactory::createAttachmentDescription
    (const VkDeviceManager& deviceManager)
{
    colorAttachment_m.format = deviceManager.getSwapChainImageFormat();
    colorAttachment_m.samples = VK_SAMPLE_COUNT_1_BIT;
    // what to do with the data in the attachment before and after rendering
    // clear the framebuffer to black before drawing a new frame
    colorAttachment_m.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment_m.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment_m.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment_m.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment_m.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment_m.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void VkRenderPassFactory::createSubPass()
{
    // which attachment to reference by its index
    colorAttachmentRef_m.attachment = 0;
    // which layout we would like the attachment to have during a subpass
    colorAttachmentRef_m.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // subpass creation
    // vulkan may support compute subpasses in the future
    subpass_m.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_m.colorAttachmentCount = 1;
    subpass_m.pColorAttachments = &colorAttachmentRef_m;
}

void VkRenderPassFactory::createRenderPass(const VkDeviceManager& deviceManager, VkRenderPass* pRenderPass)
{
    createAttachmentDescription(deviceManager);
    createSubPass();
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // attachment and subpass can be array of those;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment_m;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass_m;
    // subpass dependency
    renderPassInfo.dependencyCount = 1;
    auto dependency = createSubpassDependency();
    renderPassInfo.pDependencies = &dependency;
    if (vkCreateRenderPass(deviceManager.getDevice(), 
        &renderPassInfo, nullptr, pRenderPass) != VK_SUCCESS)
            throw std::runtime_error("failed to create render pass!");
}

VkSubpassDependency VkRenderPassFactory::createSubpassDependency()
{
    VkSubpassDependency dependency{};
    // the implicit subpass befor or after the render pass
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // my subpass
    // dstSubpass souhld be higher than srcSubpass to prevent cycles
    dependency.dstSubpass = 0;
    // the operations to wait on and the stages in which these operations occur
    dependency.srcStageMask = 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = 
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    return dependency;
}