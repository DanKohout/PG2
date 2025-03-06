
#include "app.hpp"



void App::error_callback(int error, const char* description)
{
    std::cerr << "Error no: " << error << " : " << description << std::endl;
}

// Key callback
void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    else if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        // Prepnutí stavu VSync
        vsyncEnabled = !vsyncEnabled;
        glfwSwapInterval(vsyncEnabled ? 1 : 0);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        this_inst->r = (this_inst->r >= 1.0f) ? 0.0f : this_inst->r + 0.1f;
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        this_inst->g = (this_inst->g >= 1.0f) ? 0.0f : this_inst->g + 0.1f;
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        this_inst->b = (this_inst->b >= 1.0f) ? 0.0f : this_inst->b + 0.1f;
    }
}





