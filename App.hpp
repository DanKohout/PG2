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

#include "camera.hpp"

class App {
public:
    App();

    bool init(void);
    int run(void);

    ~App();

    

    
private:

    static bool vsyncEnabled;


    GLFWwindow* window = NULL;

    static Camera camera;
    //glm::vec3 camera_movement{};

    void GetInformation(void);
    void init_assets(void);

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    static void update_projection_matrix(GLFWwindow* window);

    ShaderProgram my_shader;

    GLfloat r{ 1.0f }, g{ 0.0f }, b{ 0.0f }, a{ 1.0f };



protected:
    // projection related variables    
    int width{ 0 }, height{ 0 };
    float fov = 60.0f;
    // store projection matrix here, update only on callbacks
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();



    // all objects of the scene
    std::unordered_map<std::string, Model> scene;
    

};