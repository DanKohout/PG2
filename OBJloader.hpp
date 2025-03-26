#pragma once
#ifndef OBJloader_H
#define OBJloader_H

#include <vector>
#include <glm/fwd.hpp>

struct VertexKey {
	unsigned int posIndex, uvIndex, normalIndex;
	bool operator<(const VertexKey& other) const {
		return std::tie(posIndex, uvIndex, normalIndex) < std::tie(other.posIndex, other.uvIndex, other.normalIndex);
	}
};

bool loadOBJ(
	const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals,
	std::vector<GLuint>& out_indices
);

#endif
