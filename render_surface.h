#pragma once
#include "core/camera.h"
#include "shader.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <memory>

struct RenderSurface {
    
    RenderSurface(int w, int h) : width(w), height(h) {

        
    }

	unsigned int fbo;
    unsigned int rbo;
    
    unsigned int depthAttachmentTexture;
    unsigned int texColor;
    unsigned int texCellID;
    unsigned int texFacetID;
    unsigned int texVertexID;
    unsigned int texMeshID;

	int width, height;
    glm::vec3 backgroundColor;


	void setup();
    void bind();
    void clear();
    void render(Shader &screenShader, unsigned int quadVAO);
    void resize(int w, int h);
    void clean();

    // Add picking functions

    void setCamera(std::shared_ptr<Camera> camera) {
        _camera = camera;
    }

    void setBackgroundColor(glm::vec3 color) {
        backgroundColor = color;
    }

    Camera& getCamera() { return *_camera; }

    private:
    std::shared_ptr<Camera> _camera;

};