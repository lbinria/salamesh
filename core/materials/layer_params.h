#pragma once
#include "material_params.h"
#include "colormap.h"
#include "layer.h"
#include "opengl_helper.h"

struct MyLayerParams : MaterialParams {

	MyLayerParams() {
		sl::createTBO(buf, tbo);
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

	bool getActivatedLayer(ElementKind kind) const {
		return activatedLayers[static_cast<int>(kind)];
	}

	void setActivatedLayer(ElementKind kind, bool activated) {
		activatedLayers[static_cast<int>(kind)] = activated;
	}

	void deactivateLayer(ElementKind kind) {
		setActivatedLayer(kind, false);
	}

	bool isActivatedLayer(ElementKind kind) const {
		return activatedLayers[static_cast<int>(kind)];
	}

	std::array<bool, 7> getActivatedLayers() {
		return activatedLayers;
	}

	void setAttribute(Attribute attr, bool autorange, bool update) {

		if (isActivatedLayer(attr.getKind()) && !update)
			return;

		auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
		auto [min, max] = sl::getRange(data);

		// Activate layer
		setActivatedLayer(attr.getKind(), true);

		if (autorange)
			range = {min, max};
		
		nDims = attr.getNDims();
		setLayer(data);
	}

	void setLayer(int idx, float val) {
		setBuf(buf, idx, val);
	}

	void setLayer(std::vector<float> data) {
		setBuf(buf, data);
	}

	virtual ParamValue get(const std::string name) override { 
		if (name == "nDims") {
			return nDims;
		} else if (name == "range") {
			return range;
		} else {
			return 0.f;
		}
	}

	virtual void set(const std::string name, ParamValue value) override {
		if (name == "nDims") {
			if (auto* pVal = std::get_if<int>(&value))
				nDims = *pVal;
		} else if (name == "range") {
			if (auto* pVal = std::get_if<sl::algebra::vec2>(&value))
				range = *pVal;
		}
	}

	// TODO maybe move to ColormapLayerParams
	Colormap getColormap(ColormapLayer l) const {
		return colormaps[static_cast<int>(l)];
	}

	// TODO maybe move to ColormapLayerParams
	void setColormap(ColormapLayer l, Colormap colormap) {
		colormaps[static_cast<int>(l)] = colormap;
	}

	virtual void loadState(json &j) {

		nDims = j["n_dims"].get<int>();
		range = {j["range"][0].get<float>(), j["range"][1].get<float>()};

		// TODO colormaps
	}

	virtual void saveState(json &j) const {

		j["n_dims"] = nDims;
		j["range"] = {range.x, range.y};

		for (int k = 0; k < 7; ++k)
			j["activated_layers"][k] = activatedLayers[k];

		// TODO colormaps
	}

	virtual void setValues(MaterialParams &params) override {
		set("n_dims", params.get("n_dims"));
		set("range", params.get("range"));

		for (int k = 0; k < 7; ++k) {
			ElementKind kind =  static_cast<ElementKind>(k);
			setActivatedLayer(kind, getActivatedLayer(kind));
		}
	}

	int nDims;
	sl::algebra::vec2 range;
	std::array<bool, 7> activatedLayers{};

	Colormap colormaps[3] = {};

	protected:

	unsigned int buf, tbo; // buffer and texture buffer
};

template<int layer>
struct LayerParams : MyLayerParams {

	LayerParams() = default;


	void init() override {

	}

	virtual void apply(Shader &shader) override {
		shader.setInt("attrNDims[" + std::to_string(layer) + "]", nDims);
		shader.setFloat2("attrRange[" + std::to_string(layer) + "]", sl::algebra::vec2(range.x, range.y));

		for (int kind = 0; kind < 7; ++kind) {
			shader.setBool("activatedLayers[" + std::to_string(layer) + "][" + std::to_string(kind) + "]", activatedLayers[kind]);
		}

	}

};

struct ColormapLayerParams : public LayerParams<Layer::COLORMAP_0> {

	ColormapLayerParams() = default;


	void apply(Shader &shader) override {
		LayerParams<Layer::COLORMAP_0>::apply(shader);
		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);
		shader.setInt("colormap0Buf", 5);
		shader.setInt("colormap1Buf", 6);
		shader.setInt("colormap2Buf", 7);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, colormaps[0].tex);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, colormaps[1].tex);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, colormaps[2].tex);

		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);

		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);

		glActiveTexture(GL_TEXTURE0 + 7);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);
	}

};

struct HighlightLayerParams : public LayerParams<Layer::HIGHLIGHT> {

	void apply(Shader &shader) override {
		LayerParams<Layer::HIGHLIGHT>::apply(shader);
		shader.setFloat3("selectColor", selectColor);
		shader.setFloat3("hoverColor", hoverColor);
		shader.setInt("highlightBuf", 3);
		
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);
	}

	virtual ParamValue get(const std::string name) override { 
		if (name == "hover_color") {
			return hoverColor;
		} else if (name == "select_color") {
			return selectColor;
		} else {
			return LayerParams<Layer::HIGHLIGHT>::get(name);
		}
	}

	virtual void set(const std::string name, ParamValue value) override {
		if (name == "hover_color") {
			if (auto* pVal = std::get_if<sl::algebra::vec3>(&value))
				hoverColor = *pVal;
		} else if (name == "select_color") {
			if (auto* pVal = std::get_if<sl::algebra::vec3>(&value))
				selectColor = *pVal;
		} else {
			LayerParams<Layer::HIGHLIGHT>::set(name, value);
		}
	}

	virtual void loadState(json &j) {
		LayerParams<Layer::HIGHLIGHT>::loadState(j);

		hoverColor = {j["hover_color"][0].get<float>(), j["hover_color"][1].get<float>(), j["hover_color"][2].get<float>()};
		selectColor = {j["select_color"][0].get<float>(), j["select_color"][1].get<float>(), j["select_color"][2].get<float>()};
	}

	virtual void saveState(json &j) const {
		LayerParams<Layer::HIGHLIGHT>::saveState(j);

		j["hover_color"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["select_color"] = json::array({selectColor.x, selectColor.y, selectColor.z});
	}

	virtual void setValues(MaterialParams &params) override {
		LayerParams<Layer::HIGHLIGHT>::setValues(params);

		set("hover_color", params.get("hover_color"));
		set("select_color", params.get("select_color"));
	}

	sl::algebra::vec3 hoverColor{1.f, 1.f, 1.f};
	sl::algebra::vec3 selectColor{0.f, 0.22f, 1.f};

};

struct FilterLayerParams : public LayerParams<Layer::FILTER> {

	void apply(Shader &shader) override {
		LayerParams<Layer::FILTER>::apply(shader);
		shader.setInt("filterBuf", 4);
		
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);
	}

};