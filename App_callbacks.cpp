
#include "App.hpp"



void App::error_callback(int error, const char* description)
{
    std::cerr << "Error no: " << error << " : " << description << std::endl;
}

// Key callback
void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_F4 && action == GLFW_PRESS) {
        this_inst->toggleVsync();
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
    else if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
        auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
        app->noclipEnabled = !app->noclipEnabled;
        std::cout << (app->noclipEnabled ? "Noclip ON\n" : "Noclip OFF\n");
    }
    else if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
        app->showHUD = !app->showHUD;
        std::cout << "HUD " << (app->showHUD ? "enabled\n" : "disabled\n");
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
        if (!app->noclipEnabled && !app->isJumping) {
            app->isJumping = true;
            app->jumpVelocity = 5.0f; // po��te�n� rychlost skoku (v�t�� = vy��� skok)
        }
    }
    else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
        app->toggleFullscreen();
    }

}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));

    // Zm�na FOV podle kole�ka my�i (v�t�inou +1 nebo -1)
    this_inst->fov -= static_cast<float>(yoffset) * 2.0f;  // zm�na o 2 stupn�
    this_inst->fov = std::clamp(this_inst->fov, 30.0f, 90.0f);  // bezpe�n� hodnoty

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
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->width = width;
    app->height = height;
    glViewport(0, 0, width, height);              // nastav� viewport na novou velikost okna
    app->update_projection_matrix(window);        // p�epo��t� perspektivn� projekci
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
        if (app) {
            app->flashlightOn = !app->flashlightOn;
            std::cout << "Flashlight toggled: " << (app->flashlightOn ? "ON" : "OFF") << std::endl;
        }
    }
}
