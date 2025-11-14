#pragma once

#ifdef _WIN32
#include "../include/glew/include/GL/glew.h"
#else
#include "../include/glad/glad.h"
#endif

namespace sl {

	void createTBO(GLuint &buf, GLuint &tex, int texId, GLenum internalFormat = GL_R32F) {
		glGenBuffers(1, &buf);
		glGenTextures(1, &tex);
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glActiveTexture(GL_TEXTURE0 + texId); 
		glBindTexture(GL_TEXTURE_BUFFER, tex);
		glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buf);
	}

}