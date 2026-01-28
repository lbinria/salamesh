#pragma once

#include "../include/stb_image.h"
#include "../include/stb_image_write.h"

#ifdef _WIN32
#include "../include/glew/include/GL/glew.h"
#else
#include "../include/glad/glad.h"
#endif

namespace sl {

	// // TODO to return type bool
	// inline void load_texture_1d(const std::string &path, unsigned int & texture, int & width, int & height, int & nChannels) {
	// 	glGenTextures(1, &texture);
	// 	glBindTexture(GL_TEXTURE_1D, texture);

	// 	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	// 	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// 	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 	unsigned char *new_data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
		
	// 	if (!new_data) {
	// 		std::cout << "Failed to load texture" << std::endl;
	// 		return;
	// 	}

	// 	GLenum format = GL_RGB;
	// 	if (nChannels == 1)
	// 		format = GL_RED;
	// 	else if (nChannels == 3)
	// 		format = GL_RGB;
	// 	else if (nChannels == 4)
	// 		format = GL_RGBA;

	// 	std::cout << "Load: " << path << ", w: " << width << ", h: " << height << ", channels: " << nChannels << std::endl;
	// 	glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format, GL_UNSIGNED_BYTE, new_data);

	// 	stbi_image_free(new_data);
	// }

	inline bool load_texture_2d(const std::string &path, unsigned int & texture, int & width, int & height, int & nChannels) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(1);
		unsigned char *new_data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
		
		if (!new_data) {
			std::cerr << "Failed to load texture: " << path << std::endl;
			return false;
		}

		std::cout << "Loaded texture: " << path << std::endl;
		std::cout << "w: " << width << ", h: " << height << ", channels: " << nChannels << std::endl;

		GLenum format = GL_RGB;
		if (nChannels == 1)
			format = GL_RED;
		else if (nChannels == 3)
			format = GL_RGB;
		else if (nChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, new_data);
		
		stbi_image_free(new_data);

		return true;
	}

	// Buffers
	inline void createTBO(GLuint &buf, GLuint &tex, GLenum internalFormat = GL_R32F) {
		glGenBuffers(1, &buf);
		glGenTextures(1, &tex);
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBindTexture(GL_TEXTURE_BUFFER, tex); // Link tex to buf
		glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buf);
	}

	// VBO
	inline void createVBOFloat(GLuint shaderId, const GLchar *name, GLsizei stride, const void *ptr) {
		GLuint sideLoc = glGetAttribLocation(shaderId, name);
		glEnableVertexAttribArray(sideLoc);
		glVertexAttribPointer(sideLoc, 1, GL_FLOAT, GL_FALSE, stride, ptr);
	}

	inline void createVBOInteger(GLuint shaderId, const GLchar *name, GLsizei stride, const void *ptr) {
		GLuint halfedgeIndexLoc = glGetAttribLocation(shaderId, name);
		glEnableVertexAttribArray(halfedgeIndexLoc);
		glVertexAttribIPointer(halfedgeIndexLoc, 1, GL_INT, stride, ptr);
	}

	inline void createVBOVec3(GLuint shaderId, const GLchar *name, GLsizei stride, const void *ptr) {
		GLuint p0Loc = glGetAttribLocation(shaderId, name);
		glEnableVertexAttribArray(p0Loc);
		glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, stride, ptr);
	}

	// UBO
	inline void createUBO(GLuint &ubo, size_t size) {
		// Init
		glGenBuffers(1, &ubo);
		// Setup
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, size);
	}

	inline void updateUBOData(GLuint &ubo, size_t size, const void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}