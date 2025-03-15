#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"
#include "assets.hpp"

bool App::vsyncEnabled = false;
Camera App::camera = Camera(glm::vec3(0, 0, 1000));

App::App()
{
    // default constructor
    // nothing to do here (so far...)
    std::cout << "Constructed...\n";
}

//callback definition for GLFW


bool App::init()
{
    try {
        glfwSetErrorCallback(error_callback);

        

        // Init GLFW :: https://www.glfw.org/documentation.html
        if (!glfwInit()) {
            return false;
        }

        // Set OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Set OpenGL profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core, comment this line for Compatible




        // open window (GL canvas) with no special properties
        // https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(800, 600, "OpenGL context", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, key_callback);



        GetInformation();
        

        //V-SYNC
        glfwSwapInterval(0);
        vsyncEnabled = false;

        // init glew
        // http://glew.sourceforge.net/basic.html
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); /* Problem: glewInit failed, something is seriously wrong. */
        }
        wglewInit();

        //glDebugMessageCallback(MessageCallback, 0);
        //glEnable(GL_DEBUG_OUTPUT);
        // initialization code
        //...

        // some init
        // if (not_success)
        //  throw std::runtime_error("something went bad");

        // set GL params
        glEnable(GL_DEPTH_TEST); // use Z buffer

        if (!GLEW_ARB_direct_state_access)
            throw std::runtime_error("No DSA :-(");
        r = 1.0f, g = 0.0f, b = 0.0f;


        glfwSetFramebufferSizeCallback(window, fbsize_callback);    // On GL framebuffer resize callback.
        glfwSetScrollCallback(window, scroll_callback);             // On mouse wheel.

        init_assets();
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }
    std::cout << "Initialized...\n";
    return true;
}

void App::init_assets(void) {
    //
    // Initialize pipeline: compile, link and use shaders
    //

    //SHADERS - define & compile & link
    my_shader = ShaderProgram("resources/basic.vert", "resources/basic.frag");

    Model my_model = Model("resources/objects/triangle.obj", my_shader);
    Model temp_model = Model("resources/objects/triangle.obj", my_shader);
    temp_model.origin.x = 10;
    scene.try_emplace("triangle", temp_model);
    temp_model.origin.x = 20;
    scene.try_emplace("triangle2", temp_model);

    //scene.insert(std::make_pair("my_first_object", my_model));//???->probably wrong
    scene.insert({ "my_first_object", my_model });
    //scene.insert("my_first_object", my_model);
    //scene.push_back("my_first_object", my_model);
    
    
}


int App::run(void)
{
    try {
        //GLfloat r, g, b, a;
        r = g = b = 0.7f; a = 1.0f;
        glm::vec4 my_rgba = { r,g,b,a };// ???-> dont know if it should be like this
        glm::vec3 rgb = { r,g,b };
        //glUseProgram(shader_prog_ID);;
        my_shader.activate();
        // ... shader MUST be active to set its uniforms!
        //my_shader.setUniform("color", rgb);
        my_shader.setUniform("uniform_color", my_rgba);

        GLint activeProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &activeProgram);
        std::cout << "Active Shader Program: " << activeProgram << std::endl;

        GLint uniform_color_location = glGetUniformLocation(my_shader.ID/*shader_prog_ID*/, "uniform_color");
        if (uniform_color_location == -1) {
            std::cerr << "Uniform location is not found in active shader program. Did you forget to activate it?\n";
        }


        //
        // Create and set projection matrix
        // You can only set uniforms AFTER shader compile 
        //
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);    // Get GL framebuffer size	

        if (height <= 0) // avoid division by 0
            height = 1;

        float ratio = static_cast<float>(width) / height;

        /*glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(60.0f), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            ratio,			     // Aspect Ratio. Depends on the size of your window.
            0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            20.0f              // 20000.0f  Far clipping plane. Keep as little as possible.
        );
        //set uniform for shaders - projection matrix
        my_shader.setUniform("uP_m", projectionMatrix);
        */
        update_projection_matrix(window);

        

        //
        // set viewport
        //
        glViewport(0, 0, width, height);
        
        camera.Position = glm::vec3(0.0f, 0.0f, 10.0f);

        //
        // set View matrix - no transformation (so far), e.g. identity matrix (unit matrix)
        //
        glm::mat4 v_m = glm::identity<glm::mat4>();
        my_shader.setUniform("uV_m", v_m);

        //
        // set Model matrix - no transformations (so far), e.g. identity matrix (unit matrix)
        //
        glm::mat4 m_m = glm::identity<glm::mat4>();
        my_shader.setUniform("uM_m", m_m);


        double fps_counter_seconds = 0;
        int fps_counter_frames = 0;

        while (!glfwWindowShouldClose(window))
        {
            
            // Time/FPS measure start
            auto fps_frame_start_timestamp = std::chrono::steady_clock::now();


            //set View matrix = set CAMERA
            /*glm::mat4 v_m = glm::lookAt(
                glm::vec3(0, 0, 10),//1000), // position of camera -> pozor na ten maly trojuhelnik -> bude prtavej
                glm::vec3(0, 0, 0),    // where to look
                glm::vec3(0, 1, 0)     // up direction
            );*/
            glm::mat4 v_m = camera.GetViewMatrix();

            // set uniforms for shader - common for all objects (do not set for each object individually, they use same shader anyway)
            my_shader.setUniform("uV_m", v_m);
            my_shader.setUniform("uP_m", projection_matrix);

            //my_shader.setUniform("uniform_color", glm::vec4(glm::sin(float(glfwGetTime()))),g,b,a)); -> postupne menici cervena
            glClearColor(0.5f, 0.1f, 0.1f, 0.9f);
            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Dynamically change color based on time
            //double time = glfwGetTime();
            //r = static_cast<float>((sin(time) + 1) / 2);
            //g = static_cast<float>((cos(time) + 1) / 2);
            //b = static_cast<float>((sin(time * 0.5) + 1) / 2);

            glUniform4f(uniform_color_location, r, g, b, a);

            /*for (auto const& model : scene) {
                model.draw();
            }*/
            for (auto& [key, value] : scene) {
                //value.draw(my_shader);
                value.draw();
            }

            //scene.at("triangle").draw();

            // Poll for and process events
            //glfwPollEvents();

            // Swap front and back buffers
            glfwSwapBuffers(window);
            //glfwSwapBuffers(globals.window);

            // Poll for and process events
            glfwPollEvents();

            auto fps_frame_end_timestamp = std::chrono::steady_clock::now();
            std::chrono::duration<double> fps_elapsed_seconds = fps_frame_end_timestamp - fps_frame_start_timestamp;
            fps_counter_seconds += fps_elapsed_seconds.count();
            fps_counter_frames++;
            if (fps_counter_seconds >= 1) {
                //std::cout << fps_counter_frames << " FPS\n";
                std::stringstream ss;
                ss << fps_counter_frames << " FPS";
                glfwSetWindowTitle(window, ss.str().c_str());
                fps_counter_seconds = 0;
                fps_counter_frames = 0;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}


void App::update_projection_matrix(/*void*/GLFWwindow* window)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (this_inst->height < 1)
        this_inst->height = 1;   // avoid division by 0

    float ratio = static_cast<float>(this_inst->width) / this_inst->height;

    this_inst->projection_matrix = glm::perspective(
        glm::radians(this_inst->fov),   // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        200.0f             // 20000.0f Far clipping plane. Keep as little as possible.
    );
}




App::~App()
{
    my_shader.clear();
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
    // clean-up
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}

void App::GetInformation() {
    std::cout << "\n============= :: GL Info :: =============\n";
    std::cout << "GL Vendor:\t" << glGetString(GL_VENDOR) << "\n";
    std::cout << "GL Renderer:\t" << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL Version:\t" << glGetString(GL_VERSION) << "\n";
    std::cout << "GL Shading ver:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (const auto errorCode = glGetError()) {
        std::cout << "[!] Pending GL error while obtaining profile: " << errorCode << "\n";
        //return;
    }
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        std::cout << "Core profile" << "\n";
    }
    else {
        std::cout << "Compatibility profile" << "\n";
    }
    std::cout << "=========================================\n\n";
}