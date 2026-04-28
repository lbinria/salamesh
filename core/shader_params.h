#pragma once 
#include "shader.h"
#include "layer.h"
#include "graphic_api.h"

#include <variant>
#include <map>

// A group of uniforms
struct ShaderParams {

	// A simple variant to hold any data a shader might need
	using ParamValue = std::variant<float, int, bool, glm::vec2, glm::vec3, glm::vec4>;

	ShaderParams() = default;
	ShaderParams(const ShaderParams&) = delete;
	ShaderParams& operator=(const ShaderParams&) = delete;

	void apply(Shader &shader) const {
		// if (!dirty)
		// 	return;
		
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

struct MeshStyleParams : public ShaderParams {

	float getSize() const {
		return size;
	}

	void setSize(float val) {
		size = val;
		dirty = true;
	}

	float getShrink() const {
		return shrink;
	}

	void setShrink(float val) {
		shrink = val;
		dirty = true;
	}

	bool getCornerVisible() const {
		return isCornerVisible;
	}

	void setCornerVisible(bool val) {
		isCornerVisible = val;
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
				setColor(glm::vec3(*val));
			}
		} else if (name == "size") {
			if (auto val = std::get_if<float>(&value)) {
				setSize(*val);
			}
		} else if (name == "shrink") {
			if (auto val = std::get_if<float>(&value)) {
				setShrink(*val);
			}
		} else if (name == "corner_visible") {
			if (auto val = std::get_if<bool>(&value)) {
				setCornerVisible(*val);
			}
		}
	}

	ParamValue get(const std::string& name) override {
		if (name == "color") {
			return getColor();
		} else if (name == "size") {
			return getSize();
		} else if (name == "shrink") {
			return getShrink();
		} else if (name == "corner_visible") {
			return getCornerVisible();
		}
		return 0.0f;
	}

	void doApply(Shader &shader) const override {
		shader.setFloat("meshSize", size);
		shader.setFloat("meshShrink", shrink);
		shader.setInt("isCornerVisible", isCornerVisible);
		shader.setFloat3("color", color);
	}

	private:
	float size = 0.f;
	float shrink = 0.f;
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
		return spacing;
	}

	void setSpacing(float val) {
		spacing = val;
		dirty = true;
	}

	float getPadding() const {
		return padding;
	}

	void setPadding(float val) {
		padding = val;
		dirty = true;
	}

	glm::vec3 getInsideColor() const {
		return insideColor;
	}

	void setInsideColor(glm::vec3 color) {
		insideColor = color;
		dirty = true;
	}

	glm::vec3 getOutsideColor() const {
		return outsideColor;
	}

	void setOutsideColor(glm::vec3 color) {
		outsideColor = color;
		dirty = true;
	}

	void set(const std::string& name, ParamValue value) override {
		if (name == "thickness") {
			if (auto val = std::get_if<float>(&value)) {
				setThickness(*val);
			}
		} else if (name == "spacing") {
			if (auto val = std::get_if<float>(&value)) {
				setSpacing(*val);
			}
		} else if (name == "padding") {
			if (auto val = std::get_if<float>(&value)) {
				setPadding(*val);
			}
		} else if (name == "inside_color") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setInsideColor(*val);
			}
		} else if (name == "outside_color") {
			if (auto val = std::get_if<glm::vec3>(&value)) {
				setOutsideColor(*val);
			}
		}
	}

	ParamValue get(const std::string& name) override {
		if (name == "thickness") {
			return getThickness();
		} else if (name == "spacing") {
			return getSpacing();
		} else if (name == "padding") {
			return getPadding();
		} else if (name == "inside_color") {
			return getInsideColor();
		} else if (name == "outside_color") {
			return getOutsideColor();
		}
		return 0.0f;
	}


	void doApply(Shader &shader) const override {
		shader.setFloat("thickness", thickness);
		shader.setFloat("spacing", spacing);
		shader.setFloat("padding", padding);
		shader.setFloat3("uColorInside", insideColor);
		shader.setFloat3("uColorOutside", outsideColor);
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
	float spacing = 0.f;
	float padding = 0.f;
	glm::vec3 insideColor{0.0, 0.97, 0.73};
	glm::vec3 outsideColor{0.0, 0.6, 0.45};
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

	void setLayerActivation(Layer layer, ElementKind kind, bool val) {
		activatedLayers[static_cast<int>(layer)][static_cast<int>(kind)] = val;
		dirty = true;
	}

	void setLayerActivation(Layer layer, bool val) {
		for (int k = 0; k < 7; ++k)
			activatedLayers[static_cast<int>(layer)][k] = val;

		dirty = true;
	}

	void doApply(Shader &shader) const override {
		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);

		for (int l = 0; l < 3; ++l) {
			shader.setInt("attrNDims[" + std::to_string(int(l)) + "]", attrNDims[l]);
			shader.setFloat2("attrRange[" + std::to_string(int(l)) + "]", attrRange[l]);
		}

		for (int l = 0; l < 5; ++l) {
			for (int k = 0; k < 7; ++k) {
				shader.setBool("activatedLayers[" + std::to_string(int(l)) + "][" + std::to_string(int(k)) + "]", activatedLayers[l][k]);
			}
		}

	}

	private:
	int colormapTexs[3] = {-1, -1, -1};
	bool activatedLayers[5][7] = {};
	int attrNDims[3] = {1, 1, 1};
	glm::vec2 attrRange[3];


};

struct ShaderBuffer {
	std::string name; // name in shader
	GLuint tbo;
	GLuint buf;

	ShaderBuffer(const std::string name) : name(name)  {
		sl::createTBO(buf, tbo);
	}
};

// A group of shader buffers
struct ShaderBuffers {

	ShaderBuffers() = default;
	ShaderBuffers(const ShaderBuffers&) = delete;
	ShaderBuffers& operator=(const ShaderBuffers&) = delete;

	void set(const std::string name, int idx, float val) {
		auto b = buf.at(name);
		glBindBuffer(GL_TEXTURE_BUFFER, b.buf);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
	}

	void set(const std::string name, std::vector<float>& data) {
		auto b = buf.at(name);
		glBindBuffer(GL_TEXTURE_BUFFER, b.buf);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}

	void apply(Shader &shader) {
		int texUnit = 3; // Start from 3 (texUnit 0,1,2 reserved to colormap textures)
		for (auto &[k, b] : buf) {
			shader.setInt(b.name.c_str(), texUnit);
			// Set TBO as texture for texUnit
			glActiveTexture(GL_TEXTURE0 + texUnit);
			glBindTexture(GL_TEXTURE_BUFFER, b.tbo);
			++texUnit;
		}
	}

	void clean() {
		for (auto &[bufName, b] : buf) {
			glDeleteTextures(1, &b.tbo);
			glDeleteBuffers(1, &b.buf);
		}
	}
	
	protected:

	std::map<std::string, ShaderBuffer> buf;


};

struct LayerBufferGroup : public ShaderBuffers {

	LayerBufferGroup() {
		buf.emplace(layerToString(Layer::COLORMAP_0), ShaderBuffer("colormap0Buf"));
		buf.emplace(layerToString(Layer::COLORMAP_1), ShaderBuffer("colormap1Buf"));
		buf.emplace(layerToString(Layer::COLORMAP_2), ShaderBuffer("colormap2Buf"));
		buf.emplace(layerToString(Layer::HIGHLIGHT), ShaderBuffer("highlightBuf"));
		buf.emplace(layerToString(Layer::FILTER), ShaderBuffer("filterBuf"));
	}
};