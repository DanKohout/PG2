#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"
#include "assets.hpp"
cv::Mat mapa;

const int maze_width = 25;
const int maze_depth = 60;



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

		mapa = cv::Mat(maze_width, maze_depth, CV_8U); // velikost bludištì
		genLabyrinth(mapa);

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

    genLabyrinth(mapa);

    return true;
}

void App::init_assets(void) {
    my_shader = ShaderProgram("resources/basic.vert", "resources/basic.frag");

    // Pùvodní jednotkové rozmìry
    float cell_size_x = 1.0f;
    float cell_size_z = 1.0f;
    float wall_height = 5.0f;
    float block_height = 1.0f;

    // Celková velikost bludištì
    float maze_scale_x = mapa.cols * cell_size_x;
    float maze_scale_z = mapa.rows * cell_size_z;

    // Posun na støed
    float offset_x = -maze_scale_x / 2.0f;
    float offset_z = -maze_scale_z / 2.0f;

    //
    // PODLAHA
    //
    Model world_floor = Model("resources/objects/floor.obj", my_shader, "./resources/textures/maze_ground.jpg");
    world_floor.transparent = false;
    world_floor.origin = glm::vec3(0.0f, 0.0f, 0.0f);
    world_floor.scale = glm::vec3(2.0f, 1.0f, 2.0f);
    scene.try_emplace("world_floor", world_floor);

    //
	// KRÁLÍK (pod podlahou) - EASTER EGG
    //
    Model model_bunny = Model("resources/objects/bunny_tri_vnt.obj", my_shader, "./resources/textures/box_rgb888.png");
    model_bunny.origin.y = -10.0f;
    scene.try_emplace("bunny", model_bunny);
    /*
    // SLUNÍÈKO – viditelný zdroj svìtla
    Model sun = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "resources/textures/sun_texture.png");
    sun.transparent = false;
    sun.origin = glm::vec3(-4.0f, 6.0f, -4.0f);  // stejný smìr jako pùvodní svìtlo
    sun.scale = glm::vec3(0.8f);                // zmenšení
    scene.try_emplace("sun_object", sun);
    */

    //
    // TESTOVACÍ KOSTKA
    //
    Model my_model = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "./resources/textures/box_rgb888.png");
    scene.insert({ "my_first_object", my_model });

    //
    // ŠABLONY PRO ZDI A CÍL
    //
    Model wall_template = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "./resources/textures/stone_wall.png");
    Model goal_template = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "./resources/textures/goal_box.png");

    //
    // GENEROVÁNÍ STÌN A CÍLE
    //
    for (int y = 0; y < mapa.rows; ++y) {
        for (int x = 0; x < mapa.cols; ++x) {
            uchar cell = mapa.at<uchar>(y, x);
            glm::vec3 base_pos = glm::vec3(x * cell_size_x + offset_x, 0.0f, y * cell_size_z + offset_z);

            if (cell == '#') {
                for (int h = 0; h < static_cast<int>(wall_height); ++h) {
                    Model wall = wall_template;
                    wall.origin = base_pos + glm::vec3(0.0f, h * block_height + block_height / 2.0f, 0.0f);
                    wall.scale = glm::vec3(cell_size_x, block_height, cell_size_z);
                    std::string name = "wall_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(h);
                    scene.try_emplace(name, wall);
                }
            }
            else if (cell == 'e') {
                Model goal = goal_template;
                goal.origin = base_pos + glm::vec3(0.0f, 0.5f, 0.0f);
                goal.scale = glm::vec3(cell_size_x, 1.0f, cell_size_z);
                scene.try_emplace("goal_cube", goal);
            }
        }
    }
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
            scene.at("bunny").orientation.y += deltaTime * 5.0f;  // 180 stupòù za sekundu




            glm::mat4 v_m = camera.GetViewMatrix();

            // set uniforms for shader - common for all objects (do not set for each object individually, they use same shader anyway)
            my_shader.setUniform("uV_m", v_m);
            //my_shader.setUniform("uP_m", projection_matrix);

            //my_shader.setUniform("uniform_color", glm::vec4(glm::sin(float(glfwGetTime()))),g,b,a)); -> postupne menici cervena
            glClearColor(0.85f, 0.90f, 1.0f, 1.0f); // svìtle modrá
            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Dynamically change color based on time
            //double time = glfwGetTime();
            //r = static_cast<float>((sin(time) + 1) / 2);
            //g = static_cast<float>((cos(time) + 1) / 2);
            //b = static_cast<float>((sin(time * 0.5) + 1) / 2);

            glUniform4f(uniform_color_location, r, g, b, a);
            my_shader.setUniform("light_direction", glm::normalize(glm::vec3(-1.0f, -1.0f, -0.3f)));

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
            //printí to jak jpgèka, tak pngèka
            for (auto& [name, model] : scene) {
                if (!model.transparent) {
                    // Pokud je to podlaha, použij vìtší tex_scale
                    if (name == "world_floor")
                        my_shader.setUniform("tex_scale", 20.0f);
                    else
                        my_shader.setUniform("tex_scale", 1.0f); // normální opakování pro ostatní

                    model.draw();
                }
                else {
                    transparent.emplace_back(&model);
                }
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

uchar App::getmap(cv::Mat& map, int x, int y)
{
    x = std::clamp(x, 0, map.cols - 1);
    y = std::clamp(y, 0, map.rows - 1);
    return map.at<uchar>(y, x); // Pozor! row = y, col = x
}

void App::genLabyrinth(cv::Mat& map) {
    // ROZMÌRY, MUSÍ SEDÌT S init_assets
    float maze_scale_x = float(maze_depth);
    float maze_scale_z = float(maze_width);

    cv::Point2i start_position, end_position;

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_height(1, map.rows - 2);
    std::uniform_int_distribution<int> uniform_width(1, map.cols - 2);
    std::uniform_int_distribution<int> uniform_block(0, 8); // 1/16 šance na zeï

    // Vyplnit celé bludištì
    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            map.at<uchar>(j, i) = (uniform_block(e1) == 0) ? '#' : '.';
        }
    }

    // Ohranièení
    for (int i = 0; i < map.cols; i++) {
        map.at<uchar>(0, i) = '#';
        map.at<uchar>(map.rows - 1, i) = '#';
    }
    for (int j = 0; j < map.rows; j++) {
        map.at<uchar>(j, 0) = '#';
        map.at<uchar>(j, map.cols - 1) = '#';
    }

    // Startovní pozice
    do {
        start_position.x = uniform_width(e1);
        start_position.y = uniform_height(e1);
    } while (map.at<uchar>(start_position.y, start_position.x) == '#');

    // Cílová pozice
    do {
        end_position.x = uniform_width(e1);
        end_position.y = uniform_height(e1);
    } while (end_position == start_position || map.at<uchar>(end_position.y, end_position.x) == '#');

    map.at<uchar>(end_position.y, end_position.x) = 'e';

    std::cout << "Start: " << start_position << "\nEnd: " << end_position << std::endl;

    for (int j = 0; j < map.rows; j++) {
        for (int i = 0; i < map.cols; i++) {
            if ((i == start_position.x) && (j == start_position.y))
                std::cout << 'X';
            else
                std::cout << static_cast<char>(map.at<uchar>(j, i));
        }
        std::cout << '\n';
    }

    // Zarovnaná pozice kamery na start
    float offset_x = -maze_scale_x / 2.0f;
    float offset_z = -maze_scale_z / 2.0f;

    camera.Position.x = static_cast<float>(start_position.x) + 0.5f + offset_x;
    camera.Position.z = static_cast<float>(start_position.y) + 0.5f + offset_z;
    camera.Position.y = 1.0f;
}


