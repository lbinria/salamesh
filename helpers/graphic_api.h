#pragma once

#ifdef _WIN32
#include "../include/glew/include/GL/glew.h"
#else
#include "../include/glad/glad.h"
#endif

namespace sl {

	inline void createTBO(GLuint &buf, GLuint &tex, int texId, GLenum internalFormat = GL_R32F) {
		glGenBuffers(1, &buf);
		glGenTextures(1, &tex);
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glActiveTexture(GL_TEXTURE0 + texId); 
		glBindTexture(GL_TEXTURE_BUFFER, tex);
		glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buf);
	}

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

}