#pragma once
#include "shader.h"
#include "render_surface.h"

struct SceneView {

	SceneView(int width, int height) : renderSurface(width, height) {}

	// TODO pass width, height on setup, remove from constructor
	void setup() {
		renderSurface.setBackgroundColor(backgroundColor);
		renderSurface.setup(); 
	}

	void render(Shader &screenShader, unsigned int quadVAO) {
		renderSurface.render(screenShader, quadVAO);
	}

	void clear() {
		renderSurface.clear();
	}

	void resize(int w, int h) {
		renderSurface.resize(w, h);
	}

	void clean() {
		renderSurface.clean();
	}

	void setCamera(std::shared_ptr<Camera> camera) {
		renderSurface.setCamera(camera);
	}

	RenderSurface &getRenderSurface() { return renderSurface; }

	private:
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};
	RenderSurface renderSurface;
};