#pragma once
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <unordered_map>
#include <filesystem>
#include <opencv2/opencv.hpp>

// OpenGL
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_err_callback.hpp"
#include "assets.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "camera.hpp"

class App {
public:
    App();

    bool init(void);
    int run(void);
    ~App();

    void GetInformation(void);
    void update_projection_matrix(GLFWwindow* window);

    // === Maze ===
    uchar getmap(cv::Mat& map, int x, int y);         // Pøístup do mapy
    void genLabyrinth(cv::Mat& map);                  // Generování bludištì

private:
    static bool vsyncEnabled;
    static Camera camera;

    bool showHUD = false;
    bool flashlightOn = false;
    bool noclipEnabled = false; // režim chùze výchozí
    bool isPositionBlocked(glm::vec3 position); // funkce pro kolize

    GLFWwindow* window = NULL;
    ShaderProgram my_shader;

    void init_assets(void);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    void drawText(const std::string& text, float x, float y);

    



    GLuint textureInit(const std::filesystem::path& file_name);
    GLuint gen_tex(cv::Mat& image);

protected:
    // projection
    int width{ 0 }, height{ 0 };
    float fov = 60.0f;
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();

    // scene data
    std::unordered_map<std::string, Model> scene;

    // color
    GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };
};
