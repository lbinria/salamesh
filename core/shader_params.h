#pragma once 
#include "shader.h"

struct ShaderParams {

	void update(Shader &shader) {
		if (!dirty)
			return;
		
		doUpdate(shader);
		dirty = false;
	}

	protected:

	virtual void doUpdate(Shader &shader) const = 0;
	
	bool dirty = true;
};

struct LightParams : public ShaderParams {

	void setEnabled(bool enabled) {
		_enabled = enabled;
		dirty = true;
	}

	bool getEnabled(bool enabled) const {
		return _enabled;
	}

	void doUpdate(Shader &shader) const override {
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

	virtual void setClippingMode(ClippingMode mode) {
		clippingMode = mode;
		dirty = true;
	}

	virtual void setClipping(bool enabled) {
		isClippingEnabled = enabled;
		dirty = true;
	}

	virtual void setClippingPlanePoint(glm::vec3 p) {
		clippingPlanePoint = p;
		dirty = true;
	}

	virtual void setClippingPlaneNormal(glm::vec3 n) {
		clippingPlaneNormal = n;
		dirty = true;
	}

	void setInvertClipping(bool invert) {
		isInvertClipping = invert;
		dirty = true;
	}


	void doUpdate(Shader &shader) const override {
		shader.setInt("clipping_mode", clippingMode);		
		shader.setInt("is_clipping_enabled", isClippingEnabled);
		shader.setFloat3("clipping_plane_point", clippingPlanePoint);
		shader.setFloat3("clipping_plane_normal", clippingPlaneNormal);
		shader.setInt("invert_clipping", isInvertClipping);
	}

	private:
	ClippingMode clippingMode = ClippingMode::STD;
	bool isClippingEnabled = false;
	glm::vec3 clippingPlanePoint;
	glm::vec3 clippingPlaneNormal;
	bool isInvertClipping = false;

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

	void doUpdate(Shader &shader) const override {
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

	void doUpdate(Shader &shader) const override {
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
	float thickness;
	float halfedgeSpacing = 0;
	float halfedgePadding = 0;
	glm::vec3 edgeInsideColor;
	glm::vec3 edgeOutsideColor;
};

struct PointSetParams : public ShaderParams {

	float getPointSize() const {
		return pointSize;
	}

	void setPointSize(float size) {
		pointSize = size;
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		color = c;
	}

	void doUpdate(Shader &shader) const override {
		shader.setFloat("pointSize", pointSize);
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
	float pointSize = 4.f;
	glm::vec3 color{0.23, 0.85, 0.66};

};

struct LayersParams : public ShaderParams {

	int getColormapTex(int i) const {
		return colormapsTex[i];
	}

	void setColormapTex(int i, int tex) {
		colormapsTex[i] = tex;
	}

	int getHighlightBuf() const {
		return highlightBuf;
	}

	void setHighlightBuf(int tex) {
		highlightBuf = tex;
	}

	int getFilterBuf() const {
		return filtertBuf;
	}

	void setFilterBuf(int tex) {
		filtertBuf = tex;
	}

	int getColormapBuf(int i) const {
		return colormapBufs[i];
	}

	void setColormapBuf(int i, int tex) {
		colormapBufs[i] = tex;
	}

	void doUpdate(Shader &shader) const override {
		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);
		shader.setInt("highlightBuf", 3);
		shader.setInt("filterBuf", 4);
		shader.setInt("colormap0Buf", 5);
		shader.setInt("colormap1Buf", 6);
		shader.setInt("colormap2Buf", 7);

		for (int l = 0; l < 3; ++l) {
			shader.setInt("colormapElement[" + std::to_string(int(l)) + "]", colormapElements[l]);
			shader.setInt("attrNDims[" + std::to_string(int(l)) + "]", attrNDims[l]);
			shader.setFloat2("attrRange[" + std::to_string(int(l)) + "]", attrRange[l]);
		}

		shader.setInt("highlightElement", highlightElement);
		shader.setInt("filterElement", filterElement);
	}

	private:
	int colormapsTex[3] = {-1, -1, -1};
	int colormapBufs[3] = {-1, -1, -1};
	int highlightBuf = -1;
	int filtertBuf = -1;

	int colormapElements[3] = {-1,-1,-1};
	int highlightElement = -1;
	int filterElement = -1;
	int attrNDims[3] = {1, 1, 1};
	glm::vec2 attrRange[3];


};