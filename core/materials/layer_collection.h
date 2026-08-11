#pragma once
#include "attribute.h"
#include "vec.h"
#include "layer.h"
#include "opengl_helper.h"

#include <map>
struct LayerUnit : public MaterialParams {

	LayerUnit(Layer l, ElementKind k) : l(l), k(k) {
		sl::createTBO(buf, tbo);
	}

	void init() override {

	}

	virtual void apply(Shader &shader) override {
		auto indexation = "[" + std::to_string(l) + "][" + std::to_string(k) + "]";

		shader.setInt("layers" + indexation + ".ndims", nDims);
		shader.setInt("layers" + indexation + ".repeat", repeat);
		shader.setFloat2("layers" + indexation + ".range", range);
		shader.setBool("layers" + indexation + ".activated", activated);

		// Note: offset = ElementKind::ELEMENT_KIND_COUNT = 7 because 0-6 are reserved for colormap textures
		auto texId = static_cast<int>(l) * ElementKind::ELEMENT_KIND_COUNT + static_cast<int>(k) + ElementKind::ELEMENT_KIND_COUNT;
		shader.setInt("layerBuffers" + indexation, texId); 
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

	void write(std::vector<float> data) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}

	void write(int idx, float val) {
		glBindBuffer(GL_TEXTURE_BUFFER, buf);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
	}

	void unset(bool freeMemory = true) {
		activated = false;

		if (freeMemory) {
			glInvalidateBufferData(buf);
		}
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

	void clean() {
		// Free buffer
		glDeleteBuffers(1, &buf);
		glDeleteTextures(1, &tbo);
	}

	// Fields
	int nDims = 1;
	int repeat = 1;
	sl::algebra::vec2 range{0, 0};
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
// TODO important here it is apply for each layer (CM, HL, FL) for each kind, whereas there is only one colormap to set by kind
struct ColormapLayerUnit : public LayerUnitus<Layer::COLORMAP_0> {

	ColormapLayerUnit(ElementKind k) : LayerUnitus<Layer::COLORMAP_0>(k) {}


	void apply(Shader &shader) override {
		LayerUnitus<Layer::COLORMAP_0>::apply(shader);

		int ki = static_cast<int>(k);
		shader.setInt("colormaps[" + std::to_string(ki) + "]", k);

		glActiveTexture(GL_TEXTURE0 + ki);
		glBindTexture(GL_TEXTURE_2D, colormaps[ki].tex);
	}

	Colormap getColormap() const {
		return colormaps[static_cast<int>(k)];
	}

	void setColormap(Colormap colormap) {
		colormaps[static_cast<int>(k)] = colormap;
	}

	private:
	std::array<Colormap, ElementKind::ELEMENT_KIND_COUNT> colormaps{};

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

	std::optional<Attribute> getAttribute() {
		return _bindAttr;
	}

	void setAttribute(Attribute attr) {
		setAttribute(attr, attr.getRange());
	}

	void setAttribute(Attribute attr, sl::algebra::vec2 range) {
		auto &layer = _layers.at(attr.getKind());
		auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
		layer->write(data);
		layer->range = range;
		layer->activated = true;
		_bindAttr = attr;
	}

	std::optional<Colormap> getColormap() {
		assert(l == Layer::COLORMAP_0 && "Unable to get colormap from layers highlight or filter"); // For now, must be compile time
		for (auto &[_, l] : _layers) {
			auto &lcm = static_cast<ColormapLayerUnit&>(*l);
			// Get arbitrary the first...
			return lcm.getColormap();
		}
		
		return std::nullopt;
	}

	void setColormap(Colormap colormap) {
		assert(l == Layer::COLORMAP_0 && "Unable to set colormap to layers highlight or filter"); // For now, must be compile time
		for (auto &[_, l] : _layers) {
			auto &lcm = static_cast<ColormapLayerUnit&>(*l);
			lcm.setColormap(colormap);
		}
	}

	void unset() {
		for (auto &[_, layer] : _layers)
			layer->unset();

		_bindAttr = std::nullopt;
	}

	void update() {
		if (!_bindAttr.has_value())
			return;

		auto &layer = _layers.at(_bindAttr.value().getKind());

		if (layer->activated)
			setAttribute(_bindAttr.value());
	}

	void clean() {
		// Unset layers data (free vram)
		unset();

		for (auto &[_, layer] : _layers) {
			layer->clean();
		}

		_layers.clear();
	}

	LayerUnit& operator[](ElementKind kind) {
		return *_layers[kind];
	}

	const LayerUnit& operator[](ElementKind kind) const {
		return *_layers.at(kind);
	}

	protected:
	Layer l;
	std::map<ElementKind, std::unique_ptr<LayerUnit>> _layers;

	std::optional<Attribute> _bindAttr; // last bind attribute (need when mesh update)

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

struct LayerCollection {

	LayerCollection() {
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

	void unset() {
		for (auto &[_, layer] : _layers)
			layer->unset();
	}

	void update() {
		for (auto &[_, layer] : _layers) {
			layer->update();
		}
	}

	LayerSet& operator[](Layer layer) {
		return *_layers[layer];
	}

	const LayerSet& operator[](Layer layer) const {
		return *_layers.at(layer);
	}

	void clean() {
		for (auto &[_, layer] : _layers) {
			layer->clean();
		}

		_layers.clear();
	}

	private:
	std::map<Layer, std::unique_ptr<LayerSet>> _layers;
};