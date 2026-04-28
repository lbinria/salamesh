#pragma once

#include "../data/element.h"
#include "../layer.h"

#include "../shader.h"
#include "../data/attribute.h"
#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../helpers.h"

#include "../material_instance.h"

struct Renderer {

	struct GeometricData {

		struct TextureBufferData {
			const void * data;
			size_t size;
		};

		struct VertexBufferData {
			const void * data;
			size_t size;
		};

		const void * vboBuffer; // TODO important transform to struct with size (and remove useless nelement variable)
		// VertexBufferData vboBuffer;
		std::map<std::string, TextureBufferData> texBuffers;
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

	virtual std::string getType() const = 0;

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

	void requestRemove() const {
		isRemoveRequested = true;
	}

	int getElementsCount() const { return nelements; }
	virtual GeometricData getData() = 0;
	virtual size_t getElementSize() = 0;


	// TODO replace the two function by one getGeometricStructure() that return GeometricStructure struct
	virtual std::vector<RendererElementField> getElementFields() = 0;
	virtual std::vector<std::string> getBuffers() { return {}; }
	
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
	mutable bool isRemoveRequested = false;


	int nelements = 0;


	protected: 

	private:

	std::string id;

	std::string name;

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;



};