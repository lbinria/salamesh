#include "render_surface.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void RenderSurface::setup() {
	// Framebuffer !
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Framebuffer texture
	glGenTextures(1, &texColor);
	glBindTexture(GL_TEXTURE_2D, texColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create depth texture
	glGenTextures(1, &depthAttachmentTexture);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create picking textures
	glGenTextures(1, &texCellID);
	glBindTexture(GL_TEXTURE_2D, texCellID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texFacetID);
	glBindTexture(GL_TEXTURE_2D, texFacetID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texVertexID);
	glBindTexture(GL_TEXTURE_2D, texVertexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach color attachments to FBO buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texFacetID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texCellID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, texVertexID, 0);
	// Attach depth attachments to FBO buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentTexture, 0);

	GLenum drawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, drawBuffers);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// Attach to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Picking framebuffer is not complete!" << std::endl;

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSurface::bind() {
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void RenderSurface::resize(int w, int h) {
	width = w;
	height = h;

	glViewport(0, 0, width, height);
    
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// TODO see if necessary ???!!!
	// Update color attachment texture
    glBindTexture(GL_TEXTURE_2D, texColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, texVertexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, texFacetID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, texCellID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    // Update depth/stencil renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    
    // Verify framebuffer completeness
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_camera->setScreenSize(width, height);
}

void RenderSurface::clear() {
	// Enable all three color attachments at once
	GLenum drawBufs[4] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};
	glDrawBuffers(4, drawBufs);

	// Clear attachments
	GLfloat zero[4] = { 0.f, 0.f, 0.f, 0.f };

	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.);
	glClearBufferfv(GL_COLOR, 0, zero); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 1, zero); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 2, zero); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 3, zero); // clear each float RT to 0
}

void RenderSurface::render(unsigned int quadVAO) {
	// Go back to default framebuffer to draw the screen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColor);
}

void RenderSurface::clean() {
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texColor);
	glDeleteTextures(1, &texCellID);
	glDeleteTextures(1, &texFacetID);
	glDeleteTextures(1, &texVertexID);
}