#pragma once 
#include "shader.h"
#include <variant>

struct ShaderParams {

	// A simple variant to hold any data a shader might need
	using ParamValue = std::variant<float, int, bool, glm::vec2, glm::vec3, glm::vec4>;

	ShaderParams() = default;
	ShaderParams(const ShaderParams&) = delete;
	ShaderParams& operator=(const ShaderParams&) = delete;

	void apply(Shader &shader) const {
		if (!dirty)
			return;
		
		doApply(shader);
		dirty = false;
	}

	// TODO add index operator for access to get/set

	// virtual void set(const std::string& name, ParamValue value) = 0;
	// virtual ParamValue get(const std::string& name) = 0;
	// TODO pass to virtual pure, just for testing
	virtual void set(const std::string& name, ParamValue value) {};
	virtual ParamValue get(const std::string& name) { return 0.f; };

	protected:

	virtual void doApply(Shader &shader) const = 0;
	
	mutable bool dirty = true;
};

struct StyleParams : public ShaderParams {

	void setColorUniformName(const std::string val) {
		colorUniformName = val;
	}

	void setColor(glm::vec3 val) {
		color = val;
		dirty = true;
	}

	glm::vec3 getColor() const {
		return color;
	}

	void set(const std::string& name, ParamValue value) override {
		if (name == "color") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setColor(*val);
			}
		}
	}

	ParamValue get(const std::string& name) {
		if (name == "color") {
			return color;
		}
		return 0.0f;
	}


	void doApply(Shader &shader) const override {
		shader.setFloat3(colorUniformName, color);
	}



	private:
	std::string colorUniformName = "color";
	glm::vec3 color{1.f, 1.f, 1.f};
};

struct LightParams : public ShaderParams {

	void setEnabled(bool enabled) {
		_enabled = enabled;
		dirty = true;
	}

	bool getEnabled() const {
		return _enabled;
	}

	void set(const std::string& name, ParamValue value) override {
		if (name == "enabled") {
			if (auto val = std::get_if<bool>(&value)) {
				setEnabled(*val);
			}
		}
	}

	ParamValue get(const std::string& name) override {
		if (name == "enabled") {
			return _enabled;
		}
		return 0.0f;
	}

	void doApply(Shader &shader) const override {
		shader.setFloat("is_light_enabled", _enabled); // TODO set int here !
	}

	private:
	bool _enabled = true;

};

struct ClippingParams : public ShaderParams {

	enum ClippingMode {
		CELL = 0,
		STD = 1
	};

	void setMode(ClippingMode val) {
		mode = val;
		dirty = true;
	}

	ClippingMode getMode() {
		return mode;
	}

	void setEnabled(bool val) {
		enabled = val;
		dirty = true;
	}

	bool getEnabled() {
		return enabled;
	}

	void setPlanePoint(glm::vec3 p) {
		planePoint = p;
		dirty = true;
	}

	glm::vec3 getPlanePoint() {
		return planePoint;
	}

	void setPlaneNormal(glm::vec3 n) {
		planeNormal = n;
		dirty = true;
	}

	glm::vec3 getPlaneNormal() {
		return planeNormal;
	}

	void setInvert(bool val) {
		invert = val;
		dirty = true;
	}

	bool getInvert() {
		return invert;
	}

	void set(const std::string& name, ParamValue value) override {
		if (name == "mode") {
			if (auto val = std::get_if<int>(&value)) {
				setMode(static_cast<ClippingMode>(*val));
			}
		} else if (name == "enabled") {
			if (auto val = std::get_if<bool>(&value)) {
				setEnabled(*val);
			}
		} else if (name == "plane_point") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setPlanePoint(*val);
			}
		} else if (name == "plane_normal") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setPlaneNormal(*val);
			}
		} else if (name == "invert") {
			if (auto val = std::get_if<bool>(&value)) {
				setInvert(*val);
			}
		}
	}

	ParamValue get(const std::string& name) override {
		if (name == "mode") {
			return static_cast<int>(getMode());
		} else if (name == "enabled") {
			return getEnabled();
		} else if (name == "plane_point") {
			return getPlanePoint();
		} else if (name == "plane_normal") {
			return getPlaneNormal();
		} else if (name == "invert") {
			return getInvert();
		}
		return 0.0f;
	}


	void doApply(Shader &shader) const override {
		shader.setInt("clipping_mode", mode);		
		shader.setInt("is_clipping_enabled", enabled);
		shader.setFloat3("clipping_plane_point", planePoint);
		shader.setFloat3("clipping_plane_normal", planeNormal);
		shader.setInt("invert_clipping", invert);
	}

	private:
	ClippingMode mode = ClippingMode::STD;
	bool enabled = false;
	glm::vec3 planePoint;
	glm::vec3 planeNormal;
	bool invert = false;

};

struct MeshParams : public ShaderParams {

	float getMeshSize() const {
		return meshSize;
	}

	void setMeshSize(float val) {
		meshSize = val;
		dirty = true;
	}

	float getMeshShrink() const {
		return meshShrink;
	}

	void setMeshShrink(float val) {
		meshShrink = val;
		dirty = true;
	}

	bool getCornerVisible() const {
		return isCornerVisible;
	}

	void setCornerVisible(bool val) {
		isCornerVisible = val;
		dirty = true;
	}

	int getMeshIndex() const {
		return meshIndex;
	}

	void setMeshIndex(int index) {
		meshIndex = index;
		dirty = true;
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		color = c;
		dirty = true;
	}

	void doApply(Shader &shader) const override {
		shader.setFloat("meshSize", meshSize);
		shader.setFloat("meshShrink", meshShrink);
		shader.setInt("isCornerVisible", isCornerVisible);
		shader.setInt("meshIndex", meshIndex);
		shader.setFloat3("color", color);
	}

	private:
	int meshIndex = -1;
	float meshSize = 0.f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
	glm::vec3 color{0.7f, 0.7f, 0.7f};

};

struct HalfedgeParams : public ShaderParams {

	float getThickness() const {
		return thickness;
	}

	void setThickness(float thick) {
		thickness = thick;
		dirty = true;
	}

	float getSpacing() const {
		return halfedgeSpacing;
	}

	void setSpacing(float spacing) {
		halfedgeSpacing = spacing;
		dirty = true;
	}

	float getPadding() const {
		return halfedgePadding;
	}

	void setPadding(float padding) {
		halfedgePadding = padding;
		dirty = true;
	}

	glm::vec3 getInsideColor() const {
		return edgeInsideColor;
	}

	void setInsideColor(glm::vec3 color) {
		edgeInsideColor = color;
		dirty = true;
	}

	glm::vec3 getOutsideColor() const {
		return edgeOutsideColor;
	}

	void setOutsideColor(glm::vec3 color) {
		edgeOutsideColor = color;
		dirty = true;
	}

	void doApply(Shader &shader) const override {
		shader.setFloat("thickness", thickness);
		shader.setFloat("spacing", halfedgeSpacing);
		shader.setFloat("padding", halfedgePadding);
		shader.setFloat3("uColorInside", edgeInsideColor);
		shader.setFloat3("uColorOutside", edgeOutsideColor);
	}

	// virtual void doLoadState(json &j) override {
	// 	setThickness(j["edgeSize"].get<float>());
	// 	setInsideColor({j["edgeInsideColor"][0].get<float>(), j["edgeInsideColor"][1].get<float>(), j["edgeInsideColor"][2].get<float>()});
	// 	setOutsideColor({j["edgeOutsideColor"][0].get<float>(), j["edgeOutsideColor"][1].get<float>(), j["edgeOutsideColor"][2].get<float>()});
	// }
	
	// virtual void doSaveState(json &j) const override {
	// 	j["edgeSize"] = thickness;
	// 	j["edgeInsideColor"] = json::array({edgeInsideColor.x, edgeInsideColor.y, edgeInsideColor.z});
	// 	j["edgeOutsideColor"] = json::array({edgeOutsideColor.x, edgeOutsideColor.y, edgeOutsideColor.z});
	// }

	private:
	float thickness = 2.f;
	float halfedgeSpacing = 0.f;
	float halfedgePadding = 0.f;
	glm::vec3 edgeInsideColor{0.0, 0.97, 0.73};
	glm::vec3 edgeOutsideColor{0.0, 0.6, 0.45};
};

struct PointSetParams : public ShaderParams {

	float getSize() const {
		return size;
	}

	void setSize(float val) {
		size = val;
		dirty = true;
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		color = c;
		dirty = true;
	}

	void set(const std::string& name, ParamValue value) override {
		if (name == "color") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setColor(*val);
			}
		} else if (name == "size") {
			if (auto val = std::get_if<float>(&value)) {
				setSize(*val);
			}
		}
	}

	ParamValue get(const std::string& name) override {
		if (name == "color") {
			return getColor();
		} else if (name == "size") {
			return getSize();
		}
		return 0.0f;
	}

	void doApply(Shader &shader) const override {
		shader.setFloat("pointSize", size);
		shader.setFloat3("pointColor", color);
	}

    // void doLoadState(json &j) override {
    //     setPointSize(j["pointSize"].get<float>());
    //     setColor({j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()});
    // }

    // void doSaveState(json &j) const override {
    //     j["pointSize"] = pointSize;
    //     j["pointColor"] = json::array({color.x, color.y, color.z});
    // }

	private:
	float size = 4.f;
	glm::vec3 color{0.23, 0.85, 0.66};

};

struct LayersParams : public ShaderParams {

	int getColormapTex(int i) const {
		return colormapTexs[i];
	}

	void setColormapTex(int i, int tex) {
		colormapTexs[i] = tex;
		dirty = true;
	}

	void setLayerNDims(Layer layer, int nDims) {
		// TODO check layer ? only works for colormaps...
		attrNDims[int(layer)] = nDims;
		dirty = true;
	}

	void setLayerRange(Layer layer, float min, float max) {
		// TODO check layer ? only works for colormaps...
		attrRange[int(layer)] = glm::vec2(min, max);
		dirty = true;
	}

	void setLayerElement(int element, Layer layer) {
		// I could refactor more, 
		// but we won't understand anything after that
		switch (layer)
		{
		case Layer::COLORMAP_0:
		case Layer::COLORMAP_1:
		case Layer::COLORMAP_2:
			colormapElements[int(layer)] = element;
			break;
		case Layer::HIGHLIGHT:
			highlightElement = element;
			break;
		case Layer::FILTER:
			filterElement = element;
			break;
		// Should never happen (except if all the case aren't covered)
		default:
			throw std::runtime_error(
				"setLayerElement for layer: " + 
				layerToString(layer) + 
				" is not implemented."
			);
		}
		dirty = true;
	}

	void doApply(Shader &shader) const override {
		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);

		for (int l = 0; l < 3; ++l) {
			shader.setInt("colormapElement[" + std::to_string(int(l)) + "]", colormapElements[l]);
			shader.setInt("attrNDims[" + std::to_string(int(l)) + "]", attrNDims[l]);
			shader.setFloat2("attrRange[" + std::to_string(int(l)) + "]", attrRange[l]);
		}

		shader.setInt("highlightElement", highlightElement);
		shader.setInt("filterElement", filterElement);
	}

	private:
	int colormapTexs[3] = {-1, -1, -1};

	int colormapElements[3] = {-1,-1,-1};
	int highlightElement = -1;
	int filterElement = -1;
	int attrNDims[3] = {1, 1, 1};
	glm::vec2 attrRange[3];


};

struct ShaderBuffer {
	GLuint tbo;
	GLuint buf;

	ShaderBuffer() {
		sl::createTBO(buf, tbo);
	}
};

struct ShaderBufferGroup {

	void apply(Shader &shader) {
		if (!dirty)
			return;

		int texUnit = 3; // Start from 3 (texUnit 0,1,2 reserved to colormap textures)
		for (auto &[k, b] : buf) {
			shader.setInt(k.c_str(), texUnit);
			glActiveTexture(GL_TEXTURE0 + texUnit);
			glBindTexture(GL_TEXTURE_BUFFER, b.tbo);
			++texUnit;
		}

		dirty = false;
	}
	
	protected:

	bool dirty = true;
	std::map<std::string, ShaderBuffer> buf;
	
	void writeBuf(unsigned int buf, std::vector<float> data) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
		dirty = true;
	}

	void writeBuf(unsigned int buf, int idx, float val) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
		dirty = true;
	}


};

struct LayerBufferGroup : public ShaderBufferGroup {

	LayerBufferGroup() {
		buf["highlightBuf"] = ShaderBuffer();
		buf["filterBuf"] = ShaderBuffer();
		buf["colormap0Buf"] = ShaderBuffer();
		buf["colormap1Buf"] = ShaderBuffer();
		buf["colormap2Buf"] = ShaderBuffer();
	}

	void setLayer(int idx, float val, Layer layer) {
		auto b = buf.at(binding[layer]);
		writeBuf(b.buf, idx, val);
	}

	void setLayer(std::vector<float> data, Layer layer) {
		auto b = buf.at(binding[layer]);
		writeBuf(b.buf, data);
	}

	private:

	// Binding between enum Layer to buffer's name
	std::map<Layer, std::string> binding{
		{ Layer::COLORMAP_0, "colormap0Buf"},
		{ Layer::COLORMAP_1, "colormap1Buf"},
		{ Layer::COLORMAP_2, "colormap2Buf"},
		{ Layer::HIGHLIGHT, "highlightBuf"},
		{ Layer::FILTER, "filterBuf"},
	};
};