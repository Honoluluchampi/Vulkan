#include <VkRenderPass.hpp>
#include <iostream>

VkAttachmentDescription VkRenderPassFactory::createAttachmentDescription
    (const VkDeviceManager& deviceManager)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = deviceManager.getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // what to do with the data in the attachment before and after rendering
    // clear the framebuffer to black before drawing a new frame
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    return colorAttachment;
}

VkSubpassDescription VkRenderPassFactory::createSubPass()
{
    VkAttachmentReference colorAttachmentRef{};
    // which attachment to reference by its index
    colorAttachmentRef.attachment = 0;
    // which layout we would like the attachment to have during a subpass
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // subpass creation
    VkSubpassDescription subpass{};
    // vulkan may support compute subpasses in the future
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
}

void VkRenderPassFactory::createRenderPass(const VkDeviceManager& deviceManager)
{
    VkAttachmentDescription attachment = createAttachmentDescription(deviceManager);
    VkSubpassDescription subpass = createSubPass();
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // attachment and subpass can be array of those;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &attachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    if (vkCreateRenderPass(deviceManager.getDevice(), 
        &renderPassInfo, nullptr, &renderPass_m) != VK_SUCCESS)
            throw std::runtime_error("failed to create render pass!");
}