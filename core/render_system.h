#pragma once
#include "scene_interface.h"
#include "material_instance.h"
#include "scene_view_interface.h"

struct RenderSystem {


	struct VertexBuffer {
		unsigned int vao;
		unsigned int vbo;
	};

	VertexBuffer& getVertexBuffer(Renderer &renderer);

	void render(IScene &scene);
	void render(Model &model, ISceneView &sceneView);
	void render(Renderer &renderer, ModelState &modelState, glm::mat4 &transform, int meshIndex, MaterialInstance &mat);

	void updateGeometry(Renderer &renderer);

	void updateAttr(Model &model, ModelState &modelState, MaterialInstance &mat);
	void updateLayer(Model &model, ModelState &modelState, MaterialInstance &mat);


	void clean() {

	}

	void setTextureColormap(int colormap, int texId) {
		texColormaps[colormap] = texId;
	}


	private:
	std::map<std::string, VertexBuffer> geometries; // vao, vbo per renderer
	unsigned int texColormaps[3];

};