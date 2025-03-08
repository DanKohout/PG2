#include <fstream>
#include <sstream>
#include <iostream>
#include "Model.hpp"

/*
    remarks:  This function is for the simple triangle.obj only
*/
void Model::loadOBJFile(const std::filesystem::path& filename, ShaderProgram& shader) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << std::endl;
        return;
    }

    std::vector<glm::vec3> positions;  // Temporary storage for vertex positions
    std::vector<unsigned int> indices; // Indices for drawing the triangle

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {  // Vertex position
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (prefix == "f") {  // Face indices
            unsigned int idx1, idx2, idx3;
            iss >> idx1 >> idx2 >> idx3;

            // Convert from 1-based to 0-based indexing
            indices.push_back(idx1 - 1);
            indices.push_back(idx2 - 1);
            indices.push_back(idx3 - 1);
        }
    }

    objFile.close();

    // Print vertex positions to console
    /*std::cout << "Loaded vertices from OBJ file:" << std::endl;
    for (const auto& pos : positions) {
        std::cout << "Vertex: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    }*/

    // Create a simple list of vertices (without normals or texture coordinates)
    std::vector<Vertex> vertices;
    for (const auto& pos : positions) {
        vertices.push_back({ pos, glm::vec3(0.0f), glm::vec2(0.0f) });  // Normal and TexCoords set to default
    }

    // Print vertex positions to console from vertices
    /*std::cout << "Final Vertex List (Stored in vertices):" << std::endl;
    for (const auto& vertex : vertices) {
        std::cout << "Vertex Position: " << vertex.Position.x << ", "
            << vertex.Position.y << ", " << vertex.Position.z << std::endl;
    }*/


    // Create the Mesh and add it to the Model
    //meshes.emplace_back(vertices, indices);->does not work
    meshes.emplace_back(
        GL_TRIANGLES,    // primitive_type (assuming triangles)
        shader,          // Shader reference
        vertices,        // Vertex data
        indices,         // Index data
        glm::vec3(0.0f), // Origin (default: no translation)
        glm::vec3(0.0f)  // Orientation (default: no rotation)
    );
}




/*void Model::createMeshes(ShaderProgram& shader) {
    if (meshes.empty()) {
        std::cerr << "No mesh data available to create meshes!" << std::endl;
        return;
    }

    // Convert raw data to Mesh objects
    std::vector<Mesh> newMeshes;
    for (auto& mesh : meshes) {  // Iterate over the loaded meshes
        Mesh newMesh(
            GL_TRIANGLES,  // Primitive type (OBJ files are usually triangle-based)
            shader,        // Shader program reference
            mesh.vertices, // Loaded vertex data
            mesh.indices,  // Loaded index data
            mesh.origin,   // Model position
            mesh.orientation // Model rotation
        );

        newMeshes.push_back(std::move(newMesh)); // Move the mesh to avoid copies
    }

    meshes = std::move(newMeshes); // Assign newly created meshes to the Model
}*/