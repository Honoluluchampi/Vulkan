#define GLFW_INCLUDE_VULKAN // by putting this, vulkan.h is included in glfw3.h
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h> // functions, structures, enumerations from LunarG SDK
#include <iostream>     
#include <stdexcept> // report and propagate an error
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class HelloTriangleApplication
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:
    void initWindow()// init GLFW, create window
    {
        glfwInit();
        // disable openGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // create actual window
        // 4th arg : choosing monitor, 5th : only relevant to openGL
        window_m = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }
    void initVulkan(){}
    void mainLoop()
    {
        while(!glfwWindowShouldClose(window_m)){
            glfwPollEvents();
        }
    }
    void cleanup()
    {
        // once window_m is closed, destroy resources and terminate glfw
        glfwDestroyWindow(window_m);
        glfwTerminate();
    }

    GLFWwindow* window_m;
};

int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}