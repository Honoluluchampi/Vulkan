#include "VkDebugger.hpp"
#include <iostream>

VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void *pUserData)
{
    std::cerr << "validation layer:" << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
// fix this function to control debug call back of the apps
void VkDebugger::setupDebugMessenger(VkInstance& instance)
{
    // fill in a structure with details about the meszsenger and its callback
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = 
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // Pointer to FunctionN
    createInfo.pfnUserCallback = VkDebugger::debugCallback;
    createInfo.pUserData = nullptr; // optional
    // create debug messenger
    if (createDebugUtilsMessengerEXT(instance,  &createInfo, nullptr, &debugMessenger_m) != VK_SUCCESS)
        throw std::runtime_error("faild to set up debug messenger!");
}

VkResult VkDebugger::createDebugUtilsMessengerEXT(
    VkInstance& instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        // create the extension object if its available
        // Since the debug messenger is specific to our Vulkan instance and its layers, it needs to be explicitly specified as first argument.
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        // execute creation
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VkDebugger::destroyDebugUtilsMessengerEXT(
    VkInstance& instance,
    //VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    // execute  destruction
    if (func != nullptr) func(instance, debugMessenger_m, pAllocator);
}