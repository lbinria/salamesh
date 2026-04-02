#pragma once

#include "../element.h"
#include "../layer.h"

#include "../shader.h"
#include "../attribute.h"
#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../helpers.h"

#include "../renderers/renderer_view.h"

struct Renderer {

	enum ClippingMode {
		CELL = 0,
		STD = 1
	};

	Renderer (const Renderer&) = delete;
	Renderer& operator= (const Renderer&) = delete;

	Renderer(std::string name, Shader shader) : 
		name(name.empty() ? sl::generateGuid() : name),
		shader(std::move(shader))
	{}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<Renderer, T>, "Renderer::as() can only be used with derived classes of Renderer");
		auto &x = static_cast<T&>(*this);
		return x;
	}

	// User must overwrite which element(s) are rendered by renderer
	// It can render one or more primitives (ElementKind::POINTS_ELT, ...)
	virtual int getRenderElementKind() = 0;

	virtual std::unique_ptr<RendererView> getDefaultView() = 0;

	std::shared_ptr<RendererView> getView(std::string viewName) {
		// Create default view
		if (!views.contains(viewName))
			views.insert({viewName, getDefaultView()});

		return views.at(viewName);
	}

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void render(RendererView &rv, glm::vec3 &position) {}
	virtual void clean() = 0;
	virtual void clear() = 0;




	// TODO to remove
	virtual void setClippingMode(ClippingMode mode) {
		shader.use();
		shader.setInt("clipping_mode", mode);		
	}

	virtual void setClipping(bool enabled) {
		shader.use();
		shader.setInt("is_clipping_enabled", enabled);
	}

	virtual void setClippingPlanePoint(glm::vec3 p) {
		shader.use();
		shader.setFloat3("clipping_plane_point", p);
	}

	virtual void setClippingPlaneNormal(glm::vec3 n) {
		shader.use();
		shader.setFloat3("clipping_plane_normal", n);
	}

	void setInvertClipping(bool invert) {
		shader.use();
		shader.setInt("invert_clipping", invert);
	}


	void loadState(json &j) {

		// TODO renderviews!
		doLoadState(j);
	}

	void saveState(json &j) const {
		// TODO renderviews!

		doSaveState(j);
	}

	std::string getName() { return name; }

	protected:
	// TODO to remove
	Shader shader;

	unsigned int VAO, VBO; // Buffers

	// TODO to remove
	unsigned int bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int texColormap0, texColormap1, texColormap2, tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures

	float *ptrAttr;

	// TODO rename to nelements
	int nelements = 0;

	void setPosition(glm::vec3 &position) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		// Set model to shader
		shader.use();
		shader.setMat4("model", model);
	}

	template<typename T>
	void writeVBOBuffer(std::vector<T> data, bool dynamicDraw = false) {
		auto drawType = dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		nelements = data.size();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(T), data.data(), drawType);
	}

	private:

	std::string name;

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;

	std::map<std::string, std::shared_ptr<RendererView>> views;

};