#pragma once
#include "attribute.h"
#include "vec.h"
#include "layer.h"
#include "layer_params.h"

struct LayerUnit : public MaterialParams {

	LayerUnit(Layer l, ElementKind k) : l(l), k(k) {
		sl::createTBO(buf, tbo);
	}

	void init() override {

	}

	virtual void apply(Shader &shader) override {
		auto indexation = "[" + std::to_string(l) + "][" + std::to_string(k) + "]";

		shader.setInt("ndims" + indexation, nDims);
		shader.setInt("repeats" + indexation, repeat);
		shader.setFloat2("ranges" + indexation, sl::algebra::vec2(range.x, range.y));
		shader.setBool("activateds" + indexation, activated);

		auto texId = static_cast<int>(l) * static_cast<int>(k);
		// Note: offset = ElementKind::ELEMENT_KIND_COUNT = 7 because 0-6 are reserved for colormap textures
		shader.setInt("layers" + indexation, ElementKind::ELEMENT_KIND_COUNT + texId); 
		// Set tex id for tex unit & bind TBO (=> bind buffer)
		glActiveTexture(GL_TEXTURE0 + texId);
		glBindTexture(GL_TEXTURE_BUFFER, tbo);
	}

	virtual ParamValue get(const std::string name) override { 
		if (name == "nDims") {
			return nDims;
		} else if (name == "repeat") {
			return repeat;
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
		} else if (name == "repeat") {
			if (auto* pVal = std::get_if<int>(&value))
				repeat = *pVal;
		} else if (name == "range") {
			if (auto* pVal = std::get_if<sl::algebra::vec2>(&value))
				range = *pVal;
		}
	}

	template<typename T>
	void write(std::vector<T> data) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
	}

	template<typename T>
	void write(int idx, T val) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(T), sizeof(T), &val);
	}

	virtual void loadState(json &j) {
		nDims = j["n_dims"].get<int>();
		repeat = j["repeat"].get<int>();
		range = {j["range"][0].get<float>(), j["range"][1].get<float>()};
	}

	virtual void saveState(json &j) const {
		j["n_dims"] = nDims;
		j["repeat"] = repeat;
		j["range"] = {range.x, range.y};
	}

	// TODO rename copyFrom
	virtual void setValues(MaterialParams &params) override {
		set("n_dims", params.get("n_dims"));
		set("repeat", params.get("repeat"));
		set("range", params.get("range"));
	}

	// Fields
	int nDims;
	int repeat;
	sl::algebra::vec2 range;
	bool activated = false;

	protected:
	unsigned int buf, tbo; // buffer and texture buffer

	Layer l;
	ElementKind k;



};

// TODO rename LayerUnit to LayerParams
template<int layer>
struct LayerUnitus : public LayerUnit {

	LayerUnitus(ElementKind k) : LayerUnit(static_cast<Layer>(layer), k) {
	}

};

struct HighlightLayerUnit : public LayerUnitus<Layer::HIGHLIGHT> {

	HighlightLayerUnit(ElementKind k) : LayerUnitus<Layer::HIGHLIGHT>(k) {}

	void apply(Shader &shader) override {
		LayerUnitus<Layer::HIGHLIGHT>::apply(shader);
		shader.setFloat3("selectColor", selectColor);
		shader.setFloat3("hoverColor", hoverColor);
	}

	virtual ParamValue get(const std::string name) override { 
		if (name == "hover_color") {
			return hoverColor;
		} else if (name == "select_color") {
			return selectColor;
		} else {
			return LayerUnitus<Layer::HIGHLIGHT>::get(name);
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
			LayerUnitus<Layer::HIGHLIGHT>::set(name, value);
		}
	}

	virtual void loadState(json &j) {
		LayerUnitus<Layer::HIGHLIGHT>::loadState(j);

		hoverColor = {j["hover_color"][0].get<float>(), j["hover_color"][1].get<float>(), j["hover_color"][2].get<float>()};
		selectColor = {j["select_color"][0].get<float>(), j["select_color"][1].get<float>(), j["select_color"][2].get<float>()};
	}

	virtual void saveState(json &j) const {
		LayerUnitus<Layer::HIGHLIGHT>::saveState(j);

		j["hover_color"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["select_color"] = json::array({selectColor.x, selectColor.y, selectColor.z});
	}

	virtual void setValues(MaterialParams &params) override {
		LayerUnitus<Layer::HIGHLIGHT>::setValues(params);

		set("hover_color", params.get("hover_color"));
		set("select_color", params.get("select_color"));
	}

	sl::algebra::vec3 hoverColor{1.f, 1.f, 1.f};
	sl::algebra::vec3 selectColor{0.f, 0.22f, 1.f};

};

// TODO rename to ColormapLayerParams
struct ColormapLayerUnit : public LayerUnitus<Layer::COLORMAP_0> {

	ColormapLayerUnit(ElementKind k) : LayerUnitus<Layer::COLORMAP_0>(k) {}


	void apply(Shader &shader) override {
		LayerUnitus<Layer::COLORMAP_0>::apply(shader);
		shader.setInt("colormaps[" + std::to_string(k) + "]", k);

		glActiveTexture(GL_TEXTURE0 + k);
		glBindTexture(GL_TEXTURE_2D, colormaps[k].tex);
	}

	Colormap getColormap() const {
		return colormaps[static_cast<int>(k)];
	}

	void setColormap(Colormap colormap) {
		colormaps[static_cast<int>(k)] = colormap;
	}

	private:
	Colormap colormaps[ElementKind::ELEMENT_KIND_COUNT] = {};

};

struct LayerSet {

	LayerSet(Layer l) : l(l) {
		// // Init layer objects
		// for (int i = 0; i < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++i) {
		// 	auto k = static_cast<ElementKind>(i);
		// 	_layers.emplace(k, LayerUnit(l, k));
		// }
	}

	void apply(Shader &shader) {
		for (auto &[_, l] : _layers) {
			l->apply(shader);
		}
	}

	void setAttribute(Attribute &attr, sl::algebra::vec2 range) {
		auto &layer = _layers.at(attr.getKind());

		if (layer->activated)
			return;

		auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
		layer->range = range;
		layer->write(data);
		// Activate layer
		layer->activated = true;
	}

	std::map<ElementKind, std::unique_ptr<LayerUnit>>& getLayers() {
		return _layers;
	}

	protected:
	Layer l;
	std::map<ElementKind, std::unique_ptr<LayerUnit>> _layers;


};

struct ColormapLayerSet : public LayerSet {
	ColormapLayerSet() : LayerSet(Layer::COLORMAP_0) {
		// Init layer objects
		for (int i = 0; i < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++i) {
			auto k = static_cast<ElementKind>(i);
			_layers.emplace(k, std::make_unique<ColormapLayerUnit>(k));
		}
	}
};

struct HighlightLayerSet : public LayerSet {
	HighlightLayerSet() : LayerSet(Layer::HIGHLIGHT) {
		// Init layer objects
		for (int i = 0; i < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++i) {
			auto k = static_cast<ElementKind>(i);
			_layers.emplace(k, std::make_unique<HighlightLayerUnit>(k));
		}
	}
};

struct FilterLayerSet : public LayerSet {
	FilterLayerSet() : LayerSet(Layer::FILTER) {
		// Init layer objects
		for (int i = 0; i < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++i) {
			auto k = static_cast<ElementKind>(i);
			_layers.emplace(k, std::make_unique<LayerUnit>(l, k));
		}
	}
};

struct LayerSetCollection {

	LayerSetCollection() {
		// Init layer objects
		_layers.emplace(Layer::COLORMAP_0, std::make_unique<ColormapLayerSet>());
		_layers.emplace(Layer::HIGHLIGHT, std::make_unique<HighlightLayerSet>());
		_layers.emplace(Layer::FILTER, std::make_unique<FilterLayerSet>());
	}

	virtual void apply(Shader &shader) {
		for (auto &[_, l] : _layers) {
			l->apply(shader);
		}
	}

	std::map<Layer, std::unique_ptr<LayerSet>>& getLayers() {
		return _layers;
	}

	private:
	std::map<Layer, std::unique_ptr<LayerSet>> _layers;
};

