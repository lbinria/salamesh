#pragma once
#include "scene_interface.h"
#include "material_instance.h"
#include "scene_view_interface.h"

struct Colormap {
	std::string name;
	int width, height;
	unsigned int tex;
};

struct RenderSystem {



	struct VertexBuffer {
		unsigned int vao;
		unsigned int vbo;
	};

	// struct TBO {
	// 	GLuint buf;
	// 	GLuint tbo;
	// };

	VertexBuffer& getVertexBuffer(Renderer &renderer);

	void render(IScene &scene);
	void render(Model &model, ISceneView &sceneView);
	void render(Renderer &renderer, glm::mat4 &transform, MaterialInstance &mat);

	void clean() {
		// Clear textures
		for (int i = 0; i < colormaps.size(); ++i)
			glDeleteTextures(1, &colormaps[i].tex);
	}

	void setTextureColormap(int colormap, int texId) {
		texColormaps[colormap] = texId;
	}

	void setupColormaps();

	void addColormap(const std::string name, const std::string filename);
	void removeColormap(const std::string name);

	void clearColormaps() {
		colormaps.clear();
		setupColormaps();
	}

	std::vector<Colormap> getColormaps() {
		return colormaps;
	}

	Colormap getColormap(const std::string name);
	Colormap getColormap(int idx);

	private:
	std::map<std::string, VertexBuffer> geometries;
	unsigned int texColormaps[3];

	std::vector<Colormap> colormaps;

};