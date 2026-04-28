#pragma once
#include "scene.h"
#include "material_instance.h"

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

	void render(Scene &scene);
	void render(Scene &scene, Model &model);
	void render(Renderer &renderer, ModelState &modelState, glm::mat4 &transform, int meshIndex, MaterialInstance &mat);

	bool updateGeometry(Renderer &renderer);

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