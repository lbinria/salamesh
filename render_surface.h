#pragma once
#include "core/camera.h"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include <memory>

struct RenderSurface {
    
	unsigned int fbo;
    unsigned int rbo;
    
    unsigned int depthAttachmentTexture;
    unsigned int texColor;
    unsigned int texCellID;
    unsigned int texFacetID;
    unsigned int texVertexID;

	int width, height;
    glm::vec3 backgroundColor;


	void setup();
    void bind();
    void clear();
    void resize(int w, int h);
    void clean();

    void setCamera(std::shared_ptr<Camera> camera) {
        _camera = camera;
    }

    Camera& getCamera() { return *_camera; }

    private:
    std::shared_ptr<Camera> _camera;

};