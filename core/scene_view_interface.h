#pragma once
#include "render_surface.h"
#include "models/model_view.h"

struct ISceneView {

	virtual void setup() = 0;
	virtual void render(Shader &screenShader, unsigned int quadVAO) = 0;
	virtual void clear() = 0;
	virtual void clean() = 0;

	virtual void resize(int w, int h) = 0;

	virtual void setCamera(std::shared_ptr<Camera> camera) = 0;

	virtual RenderSurface &getRenderSurface() = 0;

	virtual std::map<std::string, ModelView> getModelViews() = 0;
	virtual ModelView& getModelView(Model &model) = 0;

};