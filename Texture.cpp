
#include "Texture.hpp"


GLuint textureInit(const std::filesystem::path& file_name)
{
	cv::Mat image = cv::imread(file_name.string(), cv::IMREAD_UNCHANGED);  // Read with (potential) Alpha
	if (image.empty()) {
		throw std::runtime_error("No texture in file: " + file_name.string());
	}

	// or print warning, and generate synthetic image with checkerboard pattern 
	// using OpenCV and use as a texture replacement 

	GLuint texture = gen_tex(image);

	return texture;
}

GLuint gen_tex(cv::Mat& image)
{
	GLuint ID = 0;

	if (image.empty()) {
		throw std::runtime_error("Image empty?\n");
	}

	// Generates an OpenGL texture object
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);

	switch (image.channels()) {
	case 3:
		// Create and clear space for data - immutable format
		glTextureStorage2D(ID, 1, GL_RGB8, image.cols, image.rows);
		// Assigns the image to the OpenGL Texture object
		glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
		break;
	case 4:
		glTextureStorage2D(ID, 1, GL_RGBA8, image.cols, image.rows);
		glTextureSubImage2D(ID, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
		break;
	default:
		throw std::runtime_error("unsupported channel cnt. in texture:" + std::to_string(image.channels()));
	}

	// Configures the type of algorithm that is used to make the image smaller or bigger
	// nearest neighbor - ugly & fast 
	//glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	//glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// bilinear - nicer & slower
	//glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
	//glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear minifying
	glGenerateTextureMipmap(ID);  //Generate mipmaps now.

	// Configures the way the texture repeats
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return ID;
}