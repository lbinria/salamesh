#pragma once
#include "render_surface.h"
#include "models/model_view.h"
#include "material_instance.h"
struct ISceneView {

	virtual std::string getName() const = 0;

	virtual void setup() = 0;
	virtual void render(Shader &screenShader, unsigned int quadVAO) = 0;
	virtual void clear() = 0;
	virtual void clean() = 0;

	virtual void resize(int w, int h) = 0;

	virtual void setCamera(std::shared_ptr<Camera> camera) = 0;

	virtual RenderSurface &getRenderSurface() = 0;


	virtual MaterialInstance& getMaterial(const std::string vcName) = 0;
	virtual bool hasMaterial(const std::string rendererName) = 0;
	virtual void setMaterial(MaterialInstance &mat, const std::string rendererName) = 0;

};