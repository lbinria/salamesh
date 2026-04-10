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
	void render(Renderer &renderer, glm::mat4 &transform, MaterialInstance &mat);

	void clean() {

	}

	void setTextureColormap(int colormap, int texId) {
		texColormaps[colormap] = texId;
	}


	private:
	std::map<std::string, VertexBuffer> geometries;
	unsigned int texColormaps[3];

};