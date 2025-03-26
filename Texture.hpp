#pragma once

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <opencv2\opencv.hpp>
#include <GL/glew.h>
#include <filesystem>

// generate GL texture from image file
GLuint textureInit(const std::filesystem::path& file_name);

// generate GL texture from OpenCV image
GLuint gen_tex(cv::Mat& image);

#endif