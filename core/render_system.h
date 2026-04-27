#pragma once
#include "scene_interface.h"
#include "material_instance.h"
#include "scene_view_interface.h"

struct RenderSystem {


	struct VertexBuffer {
		unsigned int vao;
		unsigned int vbo;
	};

	struct TextureBuffer {
		unsigned int tbo;
		unsigned int buf;
	};

	struct GeometricBuffers {
		VertexBuffer vertexBuffer;
		std::map<std::string, TextureBuffer> texBuffers;
	};

	RenderSystem::GeometricBuffers& getGeometricBuffers(Renderer &renderer);

	void render(IScene &scene);
	void render(Model &model, ISceneView &sceneView);
	void render(Renderer &renderer, ModelState &modelState, glm::mat4 &transform, int meshIndex, MaterialInstance &mat);

	void updateGeometry(Renderer &renderer);

	void updateAttr(Model &model, ModelState &modelState, MaterialInstance &mat);
	void updateLayer(Model &model, ModelState &modelState, MaterialInstance &mat);


	void clean();

	void setTextureColormap(int colormap, int texId) {
		texColormaps[colormap] = texId;
	}


	private:
	std::map<std::string, GeometricBuffers> geometricBuffers;
	unsigned int texColormaps[3];

};