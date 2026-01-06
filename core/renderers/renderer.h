#pragma once

#include "../color_mode.h"
#include "../element.h"
#include "../layer.h"

#include "../shader.h"
#include "../attribute.h"
#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../helpers.h"

struct IRenderer {

	enum ClippingMode {
		CELL = 0,
		STD = 1
	};


	// User must overwrite which element(s) are supported for rendering
	virtual int getRenderElementKind() = 0;

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;

	bool isRenderElement(ElementKind kind) {
		return (getRenderElementKind() & kind) == kind;
	}

	void setTexture(unsigned int tex) { texColorMap = tex; }

	void setColormap0Texture(unsigned int tex) { texColormap0 = tex; }
	void setColormap1Texture(unsigned int tex) { texColormap1 = tex; }
	void setColormap2Texture(unsigned int tex) { texColormap2 = tex; }

	void setVisible(bool v) {
		visible = v;
	}

	bool getVisible() const {
		return visible;
	}

	IRenderer(Shader shader) : shader(std::move(shader)) {}

	// TODO see list of shader function, some are specific to mesh_renderer...
	void setModel(glm::mat4 model) {
		shader.use();
		shader.setMat4("model", model);
	}

	void setAttrElement(int element) {
		shader.use();
		shader.setInt("attrElement", element);
	}

	int getAttrRepeat() {
		return attrRepeat;
	}

	void setAttrRepeat(int n) {
		attrRepeat = n <= 0 ? 1 : n;
		shader.use();
		shader.setInt("attrRepeat", attrRepeat);
	}

	void setLayerElement(int element, Layer layer) {
		switch (layer)
		{
		case Layer::COLORMAP_0:
			shader.use();
			shader.setInt("colormapElement0", element);
			break;
		case Layer::COLORMAP_1:
			shader.use();
			shader.setInt("colormapElement1", element);
			break;
		case Layer::COLORMAP_2:
			shader.use();
			shader.setInt("colormapElement2", element);
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

	void setColorMode(ColorMode mode) {
		shader.use();
		shader.setInt("colorMode", mode);
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
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

	// Only on mesh
	float getMeshSize() const {
		return meshSize;
	}

	void setMeshSize(float val) {
		shader.use();
		shader.setFloat("meshSize", val);
		meshSize = val;
	}

	// Only on mesh
	float getMeshShrink() const {
		return meshShrink;
	}

	void setMeshShrink(float val) {
		shader.use();
		shader.setFloat("meshShrink", val);
		meshShrink = val;
	}

	bool getCornerVisible() const {
		return isCornerVisible;
	}

	void setCornerVisible(bool val) {
		shader.use();
		shader.setInt("isCornerVisible", val);
		isCornerVisible = val;
	}

	// Only on mesh
	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}

	// TODO remove
	template<typename T, int nDims>
	void map(std::vector<T>& data, int selectedDim, std::vector<float>& result) {

		if constexpr (nDims == 1) {
			result.resize(data.size());
			for (int i = 0; i < data.size(); ++i) {
				result[i] = static_cast<float>(data[i]);
			}
		} else {
			// YOu can refactor this !
			if (selectedDim == -1) {
				// Inline data
				result.resize(data.size() * nDims);
				for (int i = 0; i < data.size(); ++i) {
					for (int d = 0; d < nDims; ++d) {
						result[i * nDims + d] = static_cast<float>(data[i][d]);
					}
				}
			} else {
				result.resize(data.size());
				for (int i = 0; i < data.size(); ++i) {
					result[i] = static_cast<float>(data[i][selectedDim]);
				}
			}
		}
	}

	// TODO remove
	void setAttribute(Attribute& attr) {

		ContainerBase *ga = attr.ptr.get();

		// Prepare data
		std::vector<float> data;

		// Transform data
		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {
			map<double, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			map<float, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			map<int, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {
			map<bool, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(ga)) {
			map<vec2, 2>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(ga)) {
			map<vec3, 3>(a->data, attr.selectedDim, data);
		} else {
			throw std::runtime_error("Attribute type is not supported in `Renderer::setAttribute`.");
		}

		// Update attribute dimensions
		shader.use();
		shader.setInt("attrNDims", attr.getDims());

		// Get range (min-max)
		auto [min, max] = sl::getRange(data);

		// Update min/max
		shader.use();
		shader.setFloat2("attrRange", glm::vec2(min, max));

		// Update buffer
		glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}


	void setLayerNDims(Layer layer, int nDims) {
		shader.use();
		shader.setInt("attrNDims" + std::to_string(int(layer)), nDims);
	}

	void setLayerRange(Layer layer, float min, float max) {
		shader.use();
		shader.setFloat2("attrRange" + std::to_string(int(layer)), glm::vec2(min, max));
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
		setColor(glm::vec3(j["color"][0].get<float>(), j["color"][1].get<float>(), j["color"][2].get<float>()));
		setHoverColor(glm::vec3(j["hoverColor"][0].get<float>(), j["hoverColor"][1].get<float>(), j["hoverColor"][2].get<float>()));
		setSelectColor(glm::vec3(j["selectColor"][0].get<float>(), j["selectColor"][1].get<float>(), j["selectColor"][2].get<float>()));
		setMeshSize(j["meshSize"].get<float>());
		setMeshShrink(j["meshShrink"].get<float>());
		doLoadState(j);
	}

	void saveState(json &j) const {
		j["visible"] = visible;
		j["color"] = json::array({color.x, color.y, color.z});
		j["hoverColor"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["selectColor"] = json::array({selectColor.x, selectColor.y, selectColor.z});
		j["meshSize"] = meshSize;
		j["meshShrink"] = meshShrink;
		doSaveState(j);
	}



	Shader shader;

	protected:

	bool visible = true;

	glm::vec3 color{0.71f, 0.71f, 0.71f};
	glm::vec3 hoverColor{1.f, 1.f, 1.f};
	glm::vec3 selectColor{0.f, 0.22f, 1.f};
	float meshSize = 0.01f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
	int attrRepeat = 1;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufAttr, bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int texAttr, texColorMap, texColormap0, texColormap1, texColormap2, tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures

	float *ptrAttr;

	int nverts = 0;

	private:

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;

};