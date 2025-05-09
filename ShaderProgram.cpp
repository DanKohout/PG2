#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderProgram.hpp"

// set uniform according to name 
// https://docs.gl/gl4/glUniform

ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file, const std::filesystem::path& FS_file)
{
	std::vector<GLuint> shader_ids;

	shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
	shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));

	ID = link_shader(shader_ids);

	for (const auto s_id : shader_ids) {
		glDetachObjectARB(ID, s_id);
		glDeleteShader(s_id);
	}
}

void ShaderProgram::setUniform(const std::string& name, const float val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1f(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const int val) {
    // TODO: implement ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1i(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const double val) {
	// TODO: implement ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform1d(loc, val);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val)
{
    //TODO: get location ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform3fv(loc, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4 in_vec4) {
    // TODO: implement ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniform4fv(loc, 1, glm::value_ptr(in_vec4));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val)
{
    //TODO: get location ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
    // TODO: implement ->done?
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj)
{
    // TODO: implement ->done?
	int infoLogLength = 0;
	std::string s;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> v(infoLogLength);
		glGetShaderInfoLog(obj, infoLogLength, NULL, v.data());
		s.assign(begin(v), end(v));
	}
	return s;

}

std::string ShaderProgram::getProgramInfoLog(const GLuint obj)
{
	int infoLogLength = 0;
	std::string s;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> v(infoLogLength);
		glGetProgramInfoLog(obj, infoLogLength, NULL, v.data());
		s.assign(begin(v), end(v));
	}
	return s;
	// TODO: implement, check for result, print info & throw error (if any) ->done?

}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type)
{
	GLuint shader_h;
	shader_h = glCreateShader(type);
	if (shader_h == 0) {
		throw std::runtime_error("Failed to create shader.");
	}

	// Read shader source code
	std::string source_code = textFileRead(source_file);
	const char* source_ptr = source_code.c_str();

	// Attach source and compile
	glShaderSource(shader_h, 1, &source_ptr, nullptr);
	glCompileShader(shader_h);

	//glGetShaderiv()
	GLint cmpl_status;
	glGetShaderiv(shader_h, GL_COMPILE_STATUS, &cmpl_status);
	if (cmpl_status == GL_FALSE) {
		std::cerr << "Shader compilation failed for " << source_file << ":\n"
			<< getShaderInfoLog(shader_h);
		//std::cerr << getShaderInfoLog(shader_h);
		throw std::runtime_error("Shader compile err.\n");
	}
    // TODO: implement ->done?

	return shader_h;
}

GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids)
{
	GLuint prog_h = glCreateProgram();

	for (auto const id : shader_ids) {//I added "auto" just to try !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		glAttachShader(prog_h, id);
	}

	glLinkProgram(prog_h);
	 // TODO: implement: check link result, print info & throw error (if any)

	// check link result, display error (if any) ->done?
	GLint status;
	glGetProgramiv(prog_h, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		std::cerr << getProgramInfoLog(prog_h);
		throw std::runtime_error("Link err.\n");
	}
	return prog_h;
	
}

std::string ShaderProgram::textFileRead(const std::filesystem::path & filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error("Error opening file.\n");
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

