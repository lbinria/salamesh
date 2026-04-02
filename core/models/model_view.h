#pragma once
#include "../renderers/renderer_view.h"
#include "../renderers/renderer.h"

#include <map>
#include <string>
#include "model.h"

struct ModelView {

	ModelView(Model &m, std::map<std::string, std::shared_ptr<RendererView>> rendererViews) : rendererViews(rendererViews), model(m) {

	}

	std::map<std::string, std::shared_ptr<RendererView>> getRendererViews() {
		return rendererViews;
	}

	std::shared_ptr<RendererView> getRendererView(std::string name) {
		return rendererViews.at(name);
	}

	PointSetRendererView& getPoints() {
		return *std::static_pointer_cast<PointSetRendererView>(rendererViews.at("point_renderer"));
	}

	std::shared_ptr<MeshRendererView> getMesh() const {
		if(rendererViews.contains("mesh_renderer"))
			return std::static_pointer_cast<MeshRendererView>(rendererViews.at("mesh_renderer"));
		
		return nullptr;
	}

	bool getLightEnabled() const {
		return isLightEnabled;
	}

	void setLightEnabled(bool enabled) {
		for (auto const &[k, rv] : rendererViews)
			rv->setLightEnabled(enabled);

		isLightEnabled = enabled;
	}

	void push() {
		// Push highlight and filter attributes if they exist
		for (auto [k, isActivated] : activatedLayers) {
			if (!isActivated) 
				continue;

			auto [layer, kind] = k;
			updateLayer(layer, kind);
		}
	}

	int getSelectedAttr(ColormapLayer colormapLayer) {
		return selectedAttr[static_cast<int>(colormapLayer)];
	}

	void setSelectedAttr(int idx, ColormapLayer layer);

	void setSelectedColormap(int idx, ColormapLayer layer) {
		selectedColormap[static_cast<int>(layer)] = idx;
	}

	int getSelectedColormap(ColormapLayer layer) const {
		return selectedColormap[static_cast<int>(layer)];
	}

	bool isLayerActivated(ElementKind kind, Layer layer) {
		return activatedLayers[{layer, kind}];
	}

	void resetLayer(ElementKind kind, Layer layer);

	void setLayer(ElementKind kind, Layer layer, bool update = true);
	// TODO maybe reset parameter won't be used, so layer never need to be reseted in this context
	void unsetLayer(ElementKind kind, Layer layer, bool reset = false);

	inline void setColormap(ElementKind kind, ColormapLayer layer, bool update = true) {
		setLayer(kind, static_cast<Layer>(layer), update);
	}

	void unsetColormap(ElementKind kind, ColormapLayer colormapLayer) {
		unsetLayer(kind, static_cast<Layer>(colormapLayer));
	}

	void unsetColormaps(ColormapLayer colormapLayer) {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetColormap((ElementKind)kind, colormapLayer);
		}
	}

	void unsetColormaps(ElementKind kind) {
		for (int l = 0; l < 3; ++l)
			unsetColormap(kind, static_cast<ColormapLayer>(l));
	}

	void unsetColormaps() {
		// Unset all
		for (int l = 0; l < 3; ++l) {
			for (int i = 0; i < 7; ++i) {
				int kind = 1 << i;
				unsetColormap((ElementKind)kind, static_cast<ColormapLayer>(l));
			}
		}
	}

	inline void setHighlight(ElementKind kind, bool update = true) {
		setLayer(kind, Layer::HIGHLIGHT, update);
	}

	inline void unsetHighlight(ElementKind kind) {
		unsetLayer(kind, Layer::HIGHLIGHT);
	}

	void unsetHighlights() {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetHighlight((ElementKind)kind);
		}
	}

	inline void setFilter(ElementKind kind, bool update = true) {
		setLayer(kind, Layer::FILTER, update);
	}

	inline void unsetFilter(ElementKind kind) {
		unsetLayer(kind, Layer::FILTER);
	}

	void unsetFilters() {
		// Unset all
		for (int i = 0; i < 7; ++i) {
			int kind = 1 << i;
			unsetFilter((ElementKind)kind);
		}
	}

	// TODO maybe protected
	void setColormap0Texture(unsigned int tex) {
		for (auto const &[k, rv] : rendererViews)
			rv->setColormap0Texture(tex);
	}
	// TODO maybe protected
	void setColormap1Texture(unsigned int tex) {
		for (auto const &[k, rv] : rendererViews)
			rv->setColormap1Texture(tex);
	}
	// TODO maybe protected
	void setColormap2Texture(unsigned int tex) {
		for (auto const &[k, rv] : rendererViews)
			rv->setColormap2Texture(tex);
	}

	// Choose which attribute to bind to layer / kind
	void setLayerAttr(std::string name, Layer layer, ElementKind kind) {
		attrNameByLayerAndKind[{layer, kind}] = name;
	}

	std::string getLayerAttr(Layer layer, ElementKind kind) {
		std::tuple<Layer, ElementKind> k = {layer, kind};
		if (attrNameByLayerAndKind.contains(k))
			return attrNameByLayerAndKind[k];
		
		return defaultAttrName(layer);
	}

	inline std::string getColormapAttr(ElementKind kind, ColormapLayer layer) {
		return getLayerAttr(static_cast<Layer>(layer), kind);
	}

	inline void setColormapAttr(std::string name, ElementKind kind, ColormapLayer layer) {
		setLayerAttr(name, static_cast<Layer>(layer), kind);
	}

	inline std::string getHighlightAttr(ElementKind kind) {
		return getLayerAttr(Layer::HIGHLIGHT, kind);
	}

	inline void setHighlightAttr(std::string name, ElementKind kind) {
		setLayerAttr(name, Layer::HIGHLIGHT, kind);
	}

	inline std::string getFilterAttr(ElementKind kind) {
		return getLayerAttr(Layer::HIGHLIGHT, kind);
	}

	inline void setFilterAttr(std::string name, ElementKind kind) {
		setLayerAttr(name, Layer::FILTER, kind);
	}


	bool saveState(json &j) /*const*/ {
		j["is_light_enabled"] = isLightEnabled;
		
		// TODO save directly as arrays
		j["selected_colormap0"] = selectedColormap[0];
		j["selected_colormap1"] = selectedColormap[1];
		j["selected_colormap2"] = selectedColormap[2];
		j["selected_attr0"] = selectedAttr[0];
		j["selected_attr1"] = selectedAttr[1];
		j["selected_attr2"] = selectedAttr[2];

		j["attr_name_by_layer_and_kind"] = attrNameByLayerAndKind;
		j["activated_layers"] = activatedLayers;

		return true;
	}

	void loadState(json &j) {
		setLightEnabled(j["is_light_enabled"].get<bool>());

		// TODO load directly as array
		selectedAttr[0] = j["selected_attr0"].get<int>();
		selectedAttr[1] = j["selected_attr1"].get<int>();
		selectedAttr[2] = j["selected_attr2"].get<int>();
			
		selectedColormap[0] = j["selected_colormap0"].get<int>();
		selectedColormap[1] = j["selected_colormap1"].get<int>();
		selectedColormap[2] = j["selected_colormap2"].get<int>();

		for (auto &j : j["attr_name_by_layer_and_kind"]) {
			auto key = j[0];
			Layer l = (Layer)key[0].get<int>();
			ElementKind k = (ElementKind)key[1].get<int>();
			auto attrName = j[1].get<std::string>();
			attrNameByLayerAndKind[{l, k}] = attrName;
		}

		// Set layers
		for (auto &j : j["activated_layers"]) {
			auto key = j[0];
			Layer l = (Layer)key[0].get<int>();
			ElementKind k = (ElementKind)key[1].get<int>();
			auto val = j[1].get<bool>();
			if (val) {
				setLayer(k, l);
			}
		}
	}


	bool visible;

	protected:




	void updateLayer(Layer layer, ElementKind kind) {

		auto attrName = getLayerAttr(layer, kind);

		int nDims = model.getAttributeNDims(attrName, kind);

		// Extract selectedDim from string
		int selectedDim = -1;
		auto lbrPos = attrName.find('[');
		auto rbrPos = attrName.find(']');
		// if "attr[0]" requested for example, we would like to see selectedDim=0, so nDims of attribute = 1
		if (lbrPos != std::string::npos && rbrPos != std::string::npos) {
			selectedDim = std::stoi(attrName.substr(lbrPos + 1, rbrPos - lbrPos));
			attrName = attrName.substr(0, lbrPos);
			nDims = 1;
		}

		auto data_opt = model.getAttrData(attrName, kind, selectedDim);

		// Silent when no data ?
		// If no data => it means that attr name wasn't found
		if (!data_opt.has_value())
			return;

		auto data = data_opt.value();

		auto [min, max] = sl::getRange(data);

		for (auto const &[k, rv] : rendererViews) {
			if (rv->isRenderElement(kind)) {
				rv->setLayerRange(layer, min, max);
				rv->setLayerNDims(layer, nDims);
				rv->setLayer(data, layer);
			}
		}
	}

	private:
	Model &model;
	bool isLightEnabled;

	int selectedAttr[3] = {-1, -1, -1};
	int selectedColormap[3] = {0, 0, 0};

	std::map<std::tuple<Layer, ElementKind>, bool> activatedLayers;
	std::map<std::tuple<Layer, ElementKind>, std::string> attrNameByLayerAndKind;

	std::map<std::string, std::shared_ptr<RendererView>> rendererViews;

};