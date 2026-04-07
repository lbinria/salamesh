#pragma once
#include "scene_interface.h"
#include "material_instance.h"
#include "scene_view_interface.h"

struct RenderSystem {

	struct GeometricBuffer {
		unsigned int vao;
		unsigned int vbo;
		unsigned int vertexCount = 0;
	};

	GeometricBuffer& getGeometricBuffer(Renderer &renderer);

	void render(IScene &scene);
	void render(Model &model, ISceneView &sceneView);
	void render(Renderer &renderer, glm::mat4 &transform, MaterialInstance &mat);


	void setTextureColormap(int colormap, int texId) {
		texColormap[colormap] = texId;
	}

	private:
	std::map<std::string, GeometricBuffer> geometries;
	unsigned int texColormap[3];

};