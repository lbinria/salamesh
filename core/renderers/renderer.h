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





	void setVisible(bool v) {
		visible = v;

		if (v && shouldPush) {
			push();
		}
	}

	bool getVisible() const {
		return visible;
	}

	void setLayerElement(int element, Layer layer) {
		// I could refactor more, 
		// but we won't understand anything after that
		switch (layer)
		{
		case Layer::COLORMAP_0:
		case Layer::COLORMAP_1:
		case Layer::COLORMAP_2:
			shader.use();
			shader.setInt("colormapElement[" + std::to_string(int(layer)) + "]", element);
			break;
		case Layer::HIGHLIGHT:
			shader.use();
			shader.setInt("highlightElement", element);
			break;
		case Layer::FILTER:
			shader.use();
			shader.setInt("filterElement", element);
			break;
		// Should never happen (except if all the case aren't covered)
		default:
			throw std::runtime_error(
				"setLayerElement for layer: " + 
				layerToString(layer) + 
				" is not implemented."
			);
		}
	}



	glm::vec3 getHoverColor() const {
		return hoverColor;
	}

	void setHoverColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("hoverColor", c);
		hoverColor = c;
	}

	glm::vec3 getSelectColor() const {
		return selectColor;
	}

	void setSelectColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("selectColor", c);
		selectColor = c;
	}

	// TODO to remove
	void setLight(bool enabled) {
		shader.use();
		shader.setFloat("is_light_enabled", enabled); // TODO set int here !
	}

	void setLightFollowView(bool follow) {
		shader.use();
		shader.setInt("is_light_follow_view", follow);
	}

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



	void setLayerNDims(Layer layer, int nDims) {
		// TODO check layer ? only works for colormaps...
		shader.use();
		shader.setInt("attrNDims[" + std::to_string(int(layer)) + "]", nDims);
	}

	void setLayerRange(Layer layer, float min, float max) {
		// TODO check layer ? only works for colormaps...
		shader.use();
		shader.setFloat2("attrRange[" + std::to_string(int(layer)) + "]", glm::vec2(min, max));
	}

	// Put data to buffer
	void setBuf(unsigned int buf, std::vector<float> data) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}

	// Put data chunk to buffer
	void setBuf(unsigned int buf, int idx, float val) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
	}

	// Obtain buffer that matches with requested layer
	unsigned int getLayerBuffer(Layer layer) {
		switch (layer)
		{
		case Layer::COLORMAP_0:
			return bufColormap0;
		case Layer::COLORMAP_1:
			return bufColormap1;
		case Layer::COLORMAP_2:
			return bufColormap2;
		case Layer::HIGHLIGHT:
			return bufHighlight;
		case Layer::FILTER:
			return bufFilter;
		// Should never happen (except if all the case aren't covered)
		default:
			throw std::runtime_error(
				"getLayerBuffer for layer: " + 
				layerToString(layer) + 
				" is not implemented."
			);
		}
	}

	
	void setLayer(int idx, float val, Layer layer) {
		unsigned int buf = getLayerBuffer(layer);
		setBuf(buf, idx, val);
	}

	void setLayer(std::vector<float> data, Layer layer) {
		unsigned int buf = getLayerBuffer(layer);
		setBuf(buf, data);
	}

	void loadState(json &j) {
		setVisible(j["visible"].get<bool>());
		setHoverColor(glm::vec3(j["hoverColor"][0].get<float>(), j["hoverColor"][1].get<float>(), j["hoverColor"][2].get<float>()));
		setSelectColor(glm::vec3(j["selectColor"][0].get<float>(), j["selectColor"][1].get<float>(), j["selectColor"][2].get<float>()));

		doLoadState(j);
	}

	void saveState(json &j) const {
		j["visible"] = visible;
		j["hoverColor"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["selectColor"] = json::array({selectColor.x, selectColor.y, selectColor.z});

		doSaveState(j);
	}

	std::string getName() { return name; }

	protected:
	// TODO to remove
	Shader shader;

	bool visible = true;
	bool shouldPush = false;

	glm::vec3 hoverColor{1.f, 1.f, 1.f};
	glm::vec3 selectColor{0.f, 0.22f, 1.f};

	int attrRepeat = 1;

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