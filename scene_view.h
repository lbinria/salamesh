#pragma once
#include "core/scene_view_interface.h"

struct SceneView : public ISceneView {

	SceneView(int width, int height) : renderSurface(width, height) {}

	void setup() override {
		renderSurface.setBackgroundColor(backgroundColor);
		renderSurface.setup();
	}

	void render(Shader &screenShader, unsigned int quadVAO) override {
		renderSurface.render(screenShader, quadVAO);
	}

	void clear() override {
		renderSurface.clear();
	}

	void resize(int w, int h) override {
		renderSurface.resize(w, h);
	}

	void clean() override {
		renderSurface.clean();
	}

	void setCamera(std::shared_ptr<Camera> camera) {
		renderSurface.setCamera(camera);
	}

	RenderSurface &getRenderSurface() override { return renderSurface; }

	private:
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};
	RenderSurface renderSurface;
};