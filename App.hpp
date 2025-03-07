#pragma once
#include <iostream>
#include <chrono>
#include <stack>
#include <random>

// OpenCV (does not depend on GL)
#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW __MUST__ be first.
#include <GLFW/glfw3.h>

// OpenGL math (and other additional GL libraries, at the end)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_err_callback.hpp"
#include "assets.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"

class App {
public:
    App();

    bool init(void);
    int run(void);

    ~App();

    

    
private:

    static bool vsyncEnabled;


    GLFWwindow* window = NULL;

    void GetInformation(void);
    void init_assets(void);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    ShaderProgram my_shader;
    /*GLuint shader_prog_ID{0};//ShaderProgram my_shader
    GLuint VBO_ID{ 0 };
    GLuint VAO_ID{ 0 };*/


    GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };

    //Model my_model;
    /*std::vector<vertex> triangle_vertices =
    {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };*/

protected:
    std::unordered_map<std::string, Model> scene;
    

};