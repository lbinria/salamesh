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
#include "../material_instance.h"

struct Renderer {

	enum RendererElementType {
		RENDERER_ELEMENT_TYPE_INT,
		RENDERER_ELEMENT_TYPE_UINT,
		RENDERER_ELEMENT_TYPE_FLOAT,
		RENDERER_ELEMENT_TYPE_VEC2,
		RENDERER_ELEMENT_TYPE_VEC3
	};

	struct RendererElementField {
		std::string name;
		RendererElementType type;
		int offset;
	};

	enum RenderPrimitive {
		RENDER_POINTS,
		RENDER_LINES,
		RENDER_TRIANGLES
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

	virtual RenderPrimitive getRenderPrimitive() const = 0;

	void requestUpdate() const {
		dirty = true;
	}

	int getElementsCount() const { return nelements; }
	virtual const void * getData() {};
	virtual size_t getElementSize() { return 0; };
	virtual std::vector<RendererElementField> getElementFields() {
		return {};
	};
	virtual MaterialInstance getDefaultMaterial() {
		return MaterialInstance();
	}

	virtual std::unique_ptr<RendererView> getDefaultView() = 0;

	std::shared_ptr<RendererView> getView(std::string viewName) {
		// Create default view
		if (!views.contains(viewName))
			views.insert({viewName, getDefaultView()});

		return views.at(viewName);
	}

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(RendererView &rv, glm::vec3 &position) {}
	virtual void clean() = 0;
	virtual void clear() = 0;





	void loadState(json &j) {

		// for (auto &[k, rv] : views) {
		// 	rv->loadState(j["views"][]);
		// }

		doLoadState(j);
	}

	void saveState(json &j) const {

		for (auto &[k, rv] : views) {
			rv->saveState(j["views"][k]);
		}

		doSaveState(j);
	}

	std::string getName() { return name; }

	Shader& getShader() { return shader; }

	bool isDirty() const { return dirty; }
	void setDirty(bool val) { dirty = val; }


	protected:
	// TODO to remove
	Shader shader;
	mutable bool dirty = true;

	unsigned int VAO, VBO; // Buffers

	// TODO to remove
	unsigned int bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int texColormap0, texColormap1, texColormap2, tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures

	float *ptrAttr;

	int nelements = 0;

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