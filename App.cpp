#include <iostream>
#include <opencv2/opencv.hpp>

#include "app.hpp"
#include "assets.hpp"

// vypisování HUDu
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>




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
        glfwSetMouseButtonCallback(window, mouse_button_callback);


		mapa = cv::Mat(maze_width, maze_depth, CV_8U); // velikost bludištì
		genLabyrinth(mapa);

        //init_assets("resources"); // transparent and non-transparent models

        init_assets();

        //transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDepthFunc(GL_LEQUAL);


        // HUD
        // === ImGui Init ===
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Mùžeš zapnout styl
        ImGui::StyleColorsDark();

        // Pøiøazení backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");


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
    //my_shader = ShaderProgram("resources/basic.vert", "resources/basic.frag");
    my_shader = ShaderProgram("resources/basic.vert", "resources/advanced.frag");

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
    Model world_floor = Model("resources/objects/ground_flipped_normals.obj", my_shader, "./resources/textures/maze_ground.jpg");
    //Model world_floor = Model("resources/objects/ground_flipped_normals.obj", my_shader, "./resources/textures/blue_glass_tex.png");
    world_floor.transparent = false;
    world_floor.origin = glm::vec3(0.0f, 0.0f, 0.0f);
    world_floor.scale = glm::vec3(2.0f, 1.0f, 2.0f);
    
    scene.try_emplace("world_floor", world_floor);

    //
    // Prùhledné kostky na rohách
    //
    Model corner_cube = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "resources/textures/my_tex.png");
    corner_cube.scale = glm::vec3(1.0f);
    corner_cube.transparent = true;

    std::vector<std::pair<std::string, glm::vec3>> corners = {
        {"corner_1", glm::vec3(32.0f, 1.0f,  32.0f)},
        {"corner_2", glm::vec3(32.0f, 1.0f, -32.0f)},
        {"corner_3", glm::vec3(-32.0f, 1.0f, -32.0f)},
        {"corner_4", glm::vec3(-32.0f, 1.0f,  32.0f)}
    };

    for (auto& [name, pos] : corners) {
        Model m = corner_cube;
        m.origin = pos;
        scene.try_emplace(name, m);
    }


    //
	// KRÁLÍK (pod podlahou) - EASTER EGG
    //
    Model model_bunny = Model("resources/objects/bunny_tri_vnt.obj", my_shader, "./resources/textures/box_rgb888.png");
    model_bunny.origin.y = -10.0f;
    scene.try_emplace("bunny", model_bunny);
    
    // SLUNÍÈKO – viditelný zdroj svìtla
    Model sun = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "resources/textures/sun_texture.png");
    sun.transparent = false;
    sun.origin = glm::vec3(-4.0f, 6.0f, -4.0f);  // stejný smìr jako pùvodní svìtlo
    sun.scale = glm::vec3(0.8f);                // zmenšení
    scene.try_emplace("sun_object", sun);

    //
    // TESTOVACÍ KOSTKA
    //
    Model my_model = Model("resources/objects/cube_triangles_vnt.obj", my_shader, "./resources/textures/box_rgb888.png");
    my_model.origin.y = 0.5f;
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
        r = g = b = 0.7f; a = 1.0f;
        glm::vec4 my_rgba = { r,g,b,a };
        my_shader.activate();
        my_shader.setUniform("uniform_color", my_rgba);

        glfwGetFramebufferSize(window, &width, &height);
        update_projection_matrix(window);
        my_shader.setUniform("uP_m", projection_matrix);

        // FPS calculation variables
        double fps_timer = 0.0;
        int fps_counter_frames = 0;
        int fps_display = 0;

        float lastFrameTime = static_cast<float>(glfwGetTime());
        float speed = 5.0f;

        while (!glfwWindowShouldClose(window)) {
            float currentFrameTime = static_cast<float>(glfwGetTime());
            float deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            fps_timer += deltaTime;
            fps_counter_frames++;

            if (fps_timer >= 1.0) {
                fps_display = fps_counter_frames;
                fps_counter_frames = 0;
                fps_timer = 0.0;
            }

            std::vector<Model*> transparent;
            transparent.reserve(scene.size());

            // Získej vektor pohybu z kamery (na základì vstupu a èasu)
            glm::vec3 movement = camera.ProcessInput(window, deltaTime * speed);
            if (!noclipEnabled) movement.y = 0.0f; // vertikální pohyb zakázán mimo noclip režim

            if (!noclipEnabled) {
                glm::vec3 new_pos = camera.Position;

                // Otestuj pohyb ve smìru X a aplikuj ho, pokud nekoliduje
                glm::vec3 move_x = glm::vec3(movement.x, 0.0f, 0.0f);
                if (!isPositionBlocked(new_pos + move_x)) {
                    new_pos += move_x;
                }

                // Otestuj pohyb ve smìru Z a aplikuj ho, pokud nekoliduje
                glm::vec3 move_z = glm::vec3(0.0f, 0.0f, movement.z);
                if (!isPositionBlocked(new_pos + move_z)) {
                    new_pos += move_z;
                }

                // Aktualizuj pozici kamery
                camera.Position = new_pos;
            }
            else {
                // Pokud je aktivní noclip, ignoruj kolize a pohni se rovnou
                camera.Position += movement;
            }

            if (!noclipEnabled) {
                if (isJumping) {
                    // Aplikuj gravitaci bìhem skoku
                    jumpVelocity -= 9.81f * deltaTime;
                    camera.Position.y += jumpVelocity * deltaTime;

                    // Pokud kamera dopadne na zem, ukonèi skok
                    if (camera.Position.y <= 1.0f) {
                        camera.Position.y = 1.0f;
                        isJumping = false;
                        jumpVelocity = 0.0f;
                    }
                }
                else {
                    // Pokud není skok aktivní, drž výšku kamery na úrovni podlahy
                    camera.Position.y = 1.0f;
                }
            }


            // Animace pro testovací objekt
            if (scene.find("my_first_object") != scene.end()) {
                auto& model = scene.at("my_first_object");
                float amplitude = 2.0f;
                float anim_speed = 1.0f;
                float offset = sin(currentFrameTime * anim_speed) * amplitude;

                glm::vec3 moved_origin = model.origin;
                moved_origin.x += offset;

                model.local_model_matrix = glm::translate(glm::mat4(1.0f), moved_origin);
                model.local_model_matrix = glm::scale(model.local_model_matrix, model.scale);
            }

            // Animace králíka
            if (scene.find("bunny") != scene.end()) {
                auto& bunny = scene.at("bunny");
                bunny.local_model_matrix = glm::translate(glm::mat4(1.0f), bunny.origin);
                bunny.local_model_matrix = glm::rotate(bunny.local_model_matrix, currentFrameTime, glm::vec3(0, 1, 0));
            }

            // Sluneèní cyklus
            float angle = (currentFrameTime / 120.0f) * glm::two_pi<float>();
            float radius = 60.0f;
            glm::vec3 sun_pos = {
                radius * cos(angle),
                20.0f * sin(angle),
                radius * sin(angle)
            };
            scene.at("sun_object").origin = sun_pos;

            float brightness = glm::clamp((sun_pos.y + 5.0f) / 10.0f, 0.15f, 1.0f);
            glm::vec3 light_dir = glm::normalize(glm::vec3(0.0f) - sun_pos);
           /* my_shader.setUniform("light_direction", light_dir);

            my_shader.setUniform("ambient_intensity", glm::vec3(0.2f) * brightness);
            my_shader.setUniform("diffuse_intensity", glm::vec3(0.8f) * brightness);
            my_shader.setUniform("specular_intensity", glm::vec3(0.5f) * brightness);
            my_shader.setUniform("ambient_material", glm::vec3(0.8f));
            my_shader.setUniform("diffuse_material", glm::vec3(0.5f));
            my_shader.setUniform("specular_material", glm::vec3(1.0f));
            my_shader.setUniform("specular_shinines", 10.0f);

            my_shader.setUniform("spot_position", camera.Position);
            my_shader.setUniform("camera_position", camera.Position);

            my_shader.setUniform("spot_direction", camera.Front);

            my_shader.setUniform("spot_cutoff", cos(glm::radians(12.5f)));     // Inner cone (cosine of angle in radians)
            my_shader.setUniform("spot_outer_cutoff", cos(glm::radians(17.5f))); // Outer cone (for soft edges)
            my_shader.setUniform("spot_on", flashlightOn);
            */


            glClearColor(0.85f * brightness, 0.9f * brightness, 1.0f * brightness, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //my_shader.setUniform("ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            // Spot light
            my_shader.setUniform("uV_m", camera.GetViewMatrix());
            //my_shader.setUniform("view", camera.GetViewMatrix());
            //my_shader.setUniform("projection", projection_matrix);
            my_shader.setUniform("viewPos", camera.Position);
            my_shader.setUniform("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            my_shader.setUniform("spotLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            my_shader.setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            my_shader.setUniform("spotLight.position", camera.Position);
            my_shader.setUniform("spotLight.direction", camera.Front/* fpsCamera.getLook()*/);
            my_shader.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.0f)));
            my_shader.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(20.0f)));
            my_shader.setUniform("spotLight.constant", 1.0f);
            my_shader.setUniform("spotLight.linear", 0.07f);
            my_shader.setUniform("spotLight.exponent", 0.0017f);
            //my_shader.setUniform("spotLight.on", 1);
            my_shader.setUniform("spotOn", flashlightOn? 1:0);
            
            //else
            //    my_shader.setUniform("spotOn", 0);

            // Neprùhledné objekty
            for (auto& [name, model] : scene) {
                if (!model.transparent) {
                    my_shader.setUniform("tex_scale", (name == "world_floor") ? 20.0f : 1.0f);
                    model.draw();
                }
                else {
                    transparent.emplace_back(&model);
                }
            }

            // Prùhledné objekty
            std::sort(transparent.begin(), transparent.end(), [&](Model const* a, Model const* b) {
                glm::vec3 ta = glm::vec3(a->local_model_matrix[3]);
                glm::vec3 tb = glm::vec3(b->local_model_matrix[3]);
                return glm::distance(camera.Position, ta) < glm::distance(camera.Position, tb);
                });

            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            for (auto p : transparent) p->draw();
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            // === ImGui ===
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (showHUD) {
                ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.4f);
                ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("FPS: %d", fps_display);
                ImGui::Text("VSync: %s", vsyncEnabled ? "ON" : "OFF");
                ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
                ImGui::Text("NoClip: %s", noclipEnabled ? "ON" : "OFF");
                ImGui::Text("Flashlight: %s", flashlightOn ? "ON" : "OFF");
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
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
    // === ImGui Shutdown ===
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

bool App::isPositionBlocked(glm::vec3 pos)
{
    float player_radius = 0.3f;
    float player_height = 1.8f;
    float half_height = player_height / 2.0f;

    glm::vec3 player_min = pos - glm::vec3(player_radius, half_height, player_radius);
    glm::vec3 player_max = pos + glm::vec3(player_radius, half_height, player_radius);

    for (const auto& [name, model] : scene)
    {
        if (
            name.rfind("wall_", 0) == 0 || // všechny zdi
            name == "goal_cube" ||         // cílová kostka
            name == "my_first_object"      // rotující kostka
            )
        {
            glm::vec3 min = model.origin - model.scale * 0.5f;
            glm::vec3 max = model.origin + model.scale * 0.5f;

            // AABB-AABB kolize
            bool x_overlap = (player_max.x >= min.x && player_min.x <= max.x);
            bool y_overlap = (player_max.y >= min.y && player_min.y <= max.y);
            bool z_overlap = (player_max.z >= min.z && player_min.z <= max.z);

            if (x_overlap && y_overlap && z_overlap)
                return true;
        }
    }

    return false;
}


void App::toggleVsync() {
    vsyncEnabled = !vsyncEnabled;
    glfwMakeContextCurrent(window); // ensure correct context
    glfwSwapInterval(vsyncEnabled ? 1 : 0);
    std::cout << "VSync: " << (vsyncEnabled ? "ON" : "OFF") << std::endl;
}


