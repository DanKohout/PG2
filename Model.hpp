#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 

#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
//#include "OBJloader.hpp"

class Model
{
public:
    std::vector<Mesh> meshes;
    std::string name;
    glm::vec3 origin{};
    glm::vec3 orientation{};

    //std::vector < glm::vec3 >& out_vertices;
    //std::vector < glm::vec2 >& out_uvs;
    //std::vector < glm::vec3 >& out_normals;

    Model(const std::filesystem::path& filename, ShaderProgram& shader) {
        // load mesh (all meshes) of the model, load material of each mesh, load textures...
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
        //    notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model   
        loadOBJFile(filename, shader);  // Load mesh data from the OBJ file
        //loadMTLFile(filename);  // Load material data from the MTL file
        //createMeshes(shader);    // Create Mesh objects from loaded data
        
        /*std::string outfilename_str = filename.string(); // or outfilename.u8string()
        const char* outfilename_ptr = outfilename_str.c_str();
        loadOBJ(outfilename_ptr, out_vertices, out_uvs, out_normals);

        meshes.emplace_back(
            GL_TRIANGLES,    // primitive_type (assuming triangles)
            shader,          // Shader reference
            out_vertices,        // Vertex data
            out_indices,         // Index data
            glm::vec3(0.0f), // Origin (default: no translation)
            glm::vec3(0.0f)  // Orientation (default: no rotation)
        );*/

    }

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; s=s0+v*dt
    }

    void draw(glm::vec3 const& offset = glm::vec3(0.0), glm::vec3 const& rotation = glm::vec3(0.0f)) {


        // call draw() on mesh (all meshes)
        for (auto /*const&*/ mesh : meshes) {
            mesh.draw(origin + offset, orientation + rotation);
        }
    }
private:
    void loadOBJFile(const std::filesystem::path& filename, ShaderProgram& shader);
    /*void createMeshes(ShaderProgram& shader);*/
};

