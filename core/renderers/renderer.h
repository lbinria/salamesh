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

	struct GeometricData {
		const void * vboBuffer;
		std::vector<const void *> buffers;
	};

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
		id(sl::generateGuid()),
		name(name.empty() ? id : name),
		shader(std::move(shader))
	{}

	std::string getId() const {
		return id;
	}

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


	virtual void clear() = 0;





	void loadState(json &j) {
		doLoadState(j);
	}

	void saveState(json &j) const {
		doSaveState(j);
	}

	std::string getName() { return name; }

	Shader& getShader() { return shader; }

	bool isDirty() const { return dirty; }
	void setDirty(bool val) { dirty = val; }


	protected:
	Shader shader;
	mutable bool dirty = true;

	unsigned int VAO, VBO; // Buffers



	int nelements = 0;

	// template<typename T>
	// void writeVBOBuffer(std::vector<T> data, bool dynamicDraw = false) {
	// 	auto drawType = dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	// 	nelements = data.size();
	// 	glBindVertexArray(VAO);
	// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(T), data.data(), drawType);
	// }

	protected: 

	private:

	std::string id;

	std::string name;

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;

	std::map<std::string, std::shared_ptr<RendererView>> views;


};