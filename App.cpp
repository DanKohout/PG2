#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"
#include "assets.hpp"

bool App::vsyncEnabled = false;
Camera App::camera = Camera(glm::vec3(0, 0, 2));

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

        //Cursor "dissapear"
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



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

        std::this_thread::sleep_for(std::chrono::seconds(2));

        //glDebugMessageCallback(MessageCallback, 0);
        //glEnable(GL_DEBUG_OUTPUT);
        // initialization code
        //...

        // some init
        // if (not_success)
        //  throw std::runtime_error("something went bad");

        // set GL params
        glEnable(GL_DEPTH_TEST); // use Z buffer

        // assume ALL objects are non-transparent 
        glEnable(GL_CULL_FACE);

        if (!GLEW_ARB_direct_state_access)
            throw std::runtime_error("No DSA :-(");
        r = 1.0f, g = 0.0f, b = 0.0f;


        glfwSetFramebufferSizeCallback(window, fbsize_callback);    // On GL framebuffer resize callback.
        glfwSetScrollCallback(window, scroll_callback);             // On mouse wheel.
        glfwSetCursorPosCallback(window, cursor_position_callback);

        //init_assets("resources"); // transparent and non-transparent models

        init_assets();

        //transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDepthFunc(GL_LEQUAL);

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

    std::filesystem::path texture_file_path = "./resources/textures/box_rgb888.png";

    Model my_model = Model("resources/objects/cube_triangles_vnt.obj", my_shader, texture_file_path);

    Model model_bunny = Model("resources/objects/bunny_tri_vnt.obj", my_shader, "./resources/textures/box_rgb888.png");
    model_bunny.origin.y = -10.0;


    //Model transp_model = Model("resources/objects/bunny_tri_vnt.obj", my_shader, "./resources/textures/my_tex.png");
    Model transp_model = Model("resources/objects/floor.obj", my_shader, "./resources/textures/blue_glass_tex.png");
    transp_model.transparent = true;
    transp_model.origin.y = -0.5;

    /*Model temp_model = Model("resources/objects/triangle.obj", my_shader);
    temp_model.origin.x = 2;
    scene.try_emplace("triangle", temp_model);
    temp_model.origin.x = 4;
    scene.try_emplace("triangle2", temp_model);*/
    scene.try_emplace("bunny", transp_model);
    scene.try_emplace("model_bunny", model_bunny);
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
  
        glfwGetFramebufferSize(window, &width, &height);    // Get GL framebuffer size	

        update_projection_matrix(window);
       
        //set uniform for shaders - projection matrix
        my_shader.setUniform("uP_m", projection_matrix);
        
      

        

        //
        // set viewport
        //
        //glViewport(0, 0, width, height);
        
        camera.Position = glm::vec3(-0.3f, 0.0f, 0.0f);

        double fps_counter_seconds = 0;
        int fps_counter_frames = 0;

        float lastFrameTime = static_cast<float>(glfwGetTime()); // Store the time of the last frame

        float speed = 5;

        while (!glfwWindowShouldClose(window))
        {
            
            // Time/FPS measure start
            auto fps_frame_start_timestamp = std::chrono::steady_clock::now();

            //camera timing
            float currentFrameTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrameTime - lastFrameTime; // Time difference
            lastFrameTime = currentFrameTime; // Update lastFrameTime
            
            std::vector<Model*> transparent;    // temporary, vector of pointers to transparent objects
            transparent.reserve(scene.size());  // reserve size for all objects to avoid reallocation

            
            
            camera.Position += camera.ProcessInput(window, deltaTime*speed);


            scene.at("my_first_object").origin.x = 0.3 * sin(glfwGetTime());


            glm::mat4 v_m = camera.GetViewMatrix();

            // set uniforms for shader - common for all objects (do not set for each object individually, they use same shader anyway)
            my_shader.setUniform("uV_m", v_m);
            //my_shader.setUniform("uP_m", projection_matrix);

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
            my_shader.setUniform("light_position", glm::vec3(30.0f, 40.0f, 30.0f));

            my_shader.setUniform("ambient_intensity", glm::vec3(0.2f, 0.2f, 0.2f));
            my_shader.setUniform("diffuse_intensity", glm::vec3(0.8f, 0.8f, 0.8f));
            my_shader.setUniform("specular_intensity", glm::vec3(0.5f, 0.5f, 0.5f));

            my_shader.setUniform("ambient_material", glm::vec3(0.8f, 0.8f, 0.8f));
            my_shader.setUniform("diffuse_material", glm::vec3(0.5f, 0.5f, 0.5f));
            my_shader.setUniform("specular_material", glm::vec3(1.0f, 1.0f, 1.0f));

            my_shader.setUniform("specular_shinines", 5.0f);


            /*
            my_shader.setUniform("spot_position", glm::vec3(0.0f, 0.0f, 0.0f));
            my_shader.setUniform("spot_direction", glm::vec3(0.0f, 0.0f, -1.0f));
            my_shader.setUniform("spot_cutoff", cos(glm::radians(12.5f)));
            my_shader.setUniform("spot_outer_cutoff", cos(glm::radians(17.5f)));
            */
            // Assuming you're using glm
            /*my_shader.setUniform("spot_position", glm::vec3(0.0f)); // camera position in view space
            my_shader.setUniform("spot_direction", glm::vec3(0.0f, 0.0f, -1.0f)); // forward
            my_shader.setUniform("spot_cutoff_cos", glm::cos(glm::radians(12.5f)));
            my_shader.setUniform("spot_exponent", 10.0f); // how focused the light cone is

            // Attenuation factors
            my_shader.setUniform("constant_att", 1.0f);
            my_shader.setUniform("linear_att", 0.09f);
            my_shader.setUniform("quad_att", 0.032f);
            */
            //draw all non-transparent in any order
            for (auto& [name, model] : scene) {
                //value.draw(my_shader);
                if (!model.transparent)
                    model.draw();
                else
                    transparent.emplace_back(&model); // save pointer for painters algorithm
            }
             
            //draw only transparent - painter's algorithm (sort by distance from camera, from far to near)
            std::sort(transparent.begin(), transparent.end(), [&](Model const* a, Model const* b) {
                glm::vec3 translation_a = glm::vec3(a->local_model_matrix[3]);  // get 3 values from last column of model matrix = translation
                glm::vec3 translation_b = glm::vec3(b->local_model_matrix[3]);  // dtto for model B
                return glm::distance(camera.Position, translation_a) < glm::distance(camera.Position, translation_b); // sort by distance from camera
            });


            // set GL for transparent objects
            // TODO: from lectures
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);

            // draw sorted transparent
            for (auto p : transparent) {
                p->draw();
            }

            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);

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
        100.0f             // 20000.0f Far clipping plane. Keep as little as possible.
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