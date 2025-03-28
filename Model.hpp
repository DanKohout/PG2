#pragma once

#include <filesystem>
#include <string>
#include <vector> 
#include <glm/glm.hpp> 


#include "Vertex.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "OBJloader.hpp"
#include "Texture.hpp"

class Model
{
public:
    std::vector<Mesh> meshes;
    std::string name;

    // original position
    glm::vec3 origin{0.0};
    glm::vec3 orientation{0.0}; //rotation by x,y,z axis, in radians 
    glm::vec3 scale{1.0};
    glm::mat4 local_model_matrix{1.0}; //for complex transformations 

    //ShaderProgram& shader_model;
    GLuint tex_ID = 0;  // Texture ID for model

    /*
    /   - transparency = final fragment alpha < 1.0; this can happen usually because
    /     - model has transparent material
    /     - model has transparent texture
    /   -> when updating material or texture, check alpha and set to TRUE when needed
    */
    bool transparent{ false };

    /*Model(const std::filesystem::path& filename, ShaderProgram& shader) { //for triangle
        loadOBJFile(filename, shader);  // Load mesh data from the OBJ file
    }*/

    //NO TEXTURE
    Model(const std::filesystem::path& filename, ShaderProgram& shader) { //NO TEXTURE
        std::string outfilename_str = filename.string(); // or outfilename.u8string()
        const char* outfilename_ptr = outfilename_str.c_str();

        std::vector < glm::vec3 > out_vertices;
        std::vector < glm::vec2 > out_uvs;
        std::vector < glm::vec3 > out_normals;
        std::vector < GLuint > out_indices;
        loadOBJ(outfilename_ptr, out_vertices, out_uvs, out_normals, out_indices);

        std::vector<Vertex> meshVertices;

        for (size_t i = 0; i < out_vertices.size(); i++) {
            Vertex vertex{};
            vertex.Position = out_vertices[i];
            vertex.Normal = out_normals[i];
            vertex.TexCoords = out_uvs[i];
            meshVertices.push_back(vertex);
        }

        meshes.emplace_back(
            GL_TRIANGLES,    // primitive_type (assuming triangles)
            shader,          // Shader reference
            meshVertices,        // Vertex data
            out_indices,         // Index data
            glm::vec3(0.0f), // Origin (default: no translation)
            glm::vec3(0.0f)  // Orientation (default: no rotation)
        );
    }

    //WITH TEXTURE
    Model(const std::filesystem::path& filename, ShaderProgram& shader, const std::filesystem::path& texture_file_path) {//WITH TEXTURE
        // load mesh (all meshes) of the model, load material of each mesh, load textures...
        // TODO: call LoadOBJFile, LoadMTLFile (if exist), process data, create mesh and set its properties
        //    notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model   
        //loadOBJFile(filename, shader);  // Load mesh data from the OBJ file
        //loadMTLFile(filename);  // Load material data from the MTL file
        //createMeshes(shader);    // Create Mesh objects from loaded data
        //shader_model = shader;
        std::string outfilename_str = filename.string(); // or outfilename.u8string()
        const char* outfilename_ptr = outfilename_str.c_str();

        std::vector < glm::vec3 > out_vertices;
        std::vector < glm::vec2 > out_uvs;
        std::vector < glm::vec3 > out_normals;
        std::vector < GLuint > out_indices;
        loadOBJ(outfilename_ptr, out_vertices, out_uvs, out_normals, out_indices);
        
        std::vector<Vertex> meshVertices;

        for (size_t i = 0; i < out_vertices.size(); i++) {
            Vertex vertex{};
            vertex.Position = out_vertices[i];
            vertex.Normal = out_normals[i];
            vertex.TexCoords = out_uvs[i];
            meshVertices.push_back(vertex);
        }
        //std::filesystem::path texture_file_path = "./resources/textures/my_tex.png";
        GLuint texture_id = textureInit(texture_file_path.string().c_str());

        meshes.emplace_back(
            GL_TRIANGLES,    // primitive_type (assuming triangles)
            shader,          // Shader reference
            meshVertices,        // Vertex data
            out_indices,         // Index data
            glm::vec3(0.0f), // Origin (default: no translation)
            glm::vec3(0.0f),  // Orientation (default: no rotation)
            texture_id
        );
    }

    // update position etc. based on running time
    void update(const float delta_t) {
        // origin += glm::vec3(3,0,0) * delta_t; s=s0+v*dt
    }

    void draw(glm::vec3 const& offset = glm::vec3(0.0), 
              glm::vec3 const& rotation = glm::vec3(0.0f),
              glm::vec3 const& scale_change = glm::vec3(1.0f)) {

        // compute complete transformation
        glm::mat4 t = glm::translate(glm::mat4(1.0f), origin);
        glm::mat4 rx = glm::rotate(glm::mat4(1.0f), orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 ry = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rz = glm::rotate(glm::mat4(1.0f), orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

        glm::mat4 m_off = glm::translate(glm::mat4(1.0f), offset);
        glm::mat4 m_rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 m_ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 m_rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 m_s = glm::scale(glm::mat4(1.0f), scale_change);

        glm::mat4 model_matrix = local_model_matrix * s * rz * ry * rx * t * m_s * m_rz * m_ry * m_rx * m_off;

        //glUniformMatrix4fv(glGetUniformLocation(shader_model.ID, "uM_m"), 1, GL_FALSE, glm::value_ptr(model_matrix));

        

        // call draw() on mesh (all meshes)
        for (auto /*const&*/ mesh : meshes) {
            //mesh.draw(origin + offset, orientation + rotation);
            mesh.draw(model_matrix);  // do not forget to implement draw() overload with glm::mat4 parameter
        }
    }

    void draw(glm::mat4 const& model_matrix) {
        for (auto /*const&*/ mesh : meshes) {
            mesh.draw(local_model_matrix * model_matrix);  // do not forget to implement...
        }
    }



    


private:
    void loadOBJFile(const std::filesystem::path& filename, ShaderProgram& shader);
    /*void createMeshes(ShaderProgram& shader);*/

};

