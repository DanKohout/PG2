
#include "App.hpp"



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
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        int cursorMode = glfwGetInputMode(window, GLFW_CURSOR);

        // Toggle between normal (unlocked) and disabled (locked)
        if (cursorMode == GLFW_CURSOR_DISABLED)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Unlock and show cursor
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock and hide cursor
    }
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // get App instance
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->fov += 10 * yoffset; // yoffset is mostly +1 or -1; one degree difference in fov is not visible
    this_inst->fov = std::clamp(this_inst->fov, 20.0f, 170.0f); // limit FOV to reasonable values...

    this_inst->update_projection_matrix(window);
}


void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    static bool firstMouse = true;
    static float lastX = 400, lastY = 300;  // Assume the screen center initially

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // Call the function on your Camera object (assuming it's named 'camera')
    this_inst->camera.ProcessMouseMovement(xoffset, yoffset);
}


void App::fbsize_callback(GLFWwindow* window, int width, int height)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->width = width;
    this_inst->height = height;

    // set viewport
    glViewport(0, 0, width, height);
    //now your canvas has [0,0] in bottom left corner, and its size is [width x height] 

    this_inst->update_projection_matrix(window);
};





