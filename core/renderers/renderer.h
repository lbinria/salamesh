#pragma once

#include "../data/element_type.h"
#include "../data/layer.h"

#include "../shader.h"
#include "../data/attribute.h"

#include "material_params.h"

#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../helpers.h"

#include "../shader_buffer.h"
#include "../geometry.h"

struct Material {



	Material (const Material&) = delete;
	Material& operator= (const Material&) = delete;

	Material(std::string name, Shader shader) : 
		name(name.empty() ? sl::generateGuid() : name),
		shader(std::move(shader))
	{}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<Material, T>, "Renderer::as() can only be used with derived classes of Renderer");
		auto &x = static_cast<T&>(*this);
		return x;
	}


	virtual ShaderBuffer createShaderBuffer() {
		unsigned int vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		std::map<std::string, std::shared_ptr<MaterialParams>> params;
		return ShaderBuffer(shader, vao, vbo, params);
	};

	virtual void fill(Geometry &geometry, ShaderBuffer &shaderBuffer) {

	}

	// User must overwrite which element(s) are rendered by renderer
	// It can render one or more primitives (ElementKind::POINTS_ELT, ...)
	virtual int getRenderElementKind() = 0;

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;
	virtual void clear() = 0;

	bool isRenderElement(ElementKind kind) {
		return (getRenderElementKind() & kind) == kind;
	}

	void setVisible(bool v) {
		visible = v;

		if (v && shouldPush) {
			push();
		}
	}

	bool getVisible() const {
		return visible;
	}

	void loadState(json &j) {
		setVisible(j["visible"].get<bool>());

		doLoadState(j);
	}

	void saveState(json &j) const {
		j["visible"] = visible;

		doSaveState(j);
	}

	std::string getName() { return name; }

	std::shared_ptr<MaterialParams> getParams(const std::string name) {
		return _params.contains(name) ? _params.at(name) : nullptr;
	}

	template<typename TParams>
	std::shared_ptr<TParams> getParams(const std::string name) {
		return _params.contains(name) ? std::static_pointer_cast<TParams>(_params.at(name)) : nullptr;
	}


	Shader &getShader() { return shader; }

	protected:
	Shader shader;

	bool visible = true;
	bool shouldPush = false;

	unsigned int VAO, VBO; // Buffers

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




	std::map<std::string, std::shared_ptr<MaterialParams>> _params;


	private:

	std::string name;

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;


};