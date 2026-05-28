#pragma once
#include "material_params.h"
#include "../data/colormap.h"
#include "../data/layer.h"
#include "../utils/opengl_helper.h"

struct LayersParams : MaterialParams {


	LayersParams() {
		sl::createTBO(bufHighlight, tboHighlight);
		sl::createTBO(bufFilter, tboFilter);
		sl::createTBO(bufColormap0, tboColormap0);
		sl::createTBO(bufColormap1, tboColormap1);
		sl::createTBO(bufColormap2, tboColormap2);
	}

	void init() override {

	}

	void apply(Shader &shader) override {

		for (int layer = 0; layer < 5; ++layer) {
			shader.setInt("attrNDims[" + std::to_string(int(layer)) + "]", nDims[layer]);
			shader.setFloat2("attrRange[" + std::to_string(int(layer)) + "]", glm::vec2(range[layer].x, range[layer].y));

			// TODO important refactor this, just to do transition during refactorign
			if (layer < 3)
				shader.setInt("colormapElement[" + std::to_string(int(layer)) + "]", layerElement[layer]);
			else if (layer == 3)
			shader.setInt("highlightElement", layerElement[layer]);
			else if (layer == 4)
			shader.setInt("filterElement", layerElement[layer]);

			for (int kind = 0; kind < 7; ++kind) {
				shader.setBool("activatedLayers[" + std::to_string(layer) + "][" + std::to_string(kind) + "]", activatedLayers[layer][kind]);
			}
		}

		shader.setFloat3("selectColor", selectColor);
		shader.setFloat3("hoverColor", hoverColor);

		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);
		shader.setInt("highlightBuf", 3);
		shader.setInt("filterBuf", 4);
		shader.setInt("colormap0Buf", 5);
		shader.setInt("colormap1Buf", 6);
		shader.setInt("colormap2Buf", 7);

		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texColormaps[0]);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texColormaps[1]);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, texColormaps[2]);

		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);

		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_BUFFER, tboFilter);

		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_BUFFER, tboColormap0);

		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_BUFFER, tboColormap1);

		glActiveTexture(GL_TEXTURE0 + 7);
		glBindTexture(GL_TEXTURE_BUFFER, tboColormap2);
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

	// TODO to remove
	void setLayerElement(int element, Layer layer) {
		layerElement[static_cast<int>(layer)] = element;
	}

	void activateLayer(Layer layer, ElementKind kind) {
		activatedLayers[static_cast<int>(layer)][static_cast<int>(kind)] = true;
	}

	void deactivateLayer(Layer layer, ElementKind kind) {
		activatedLayers[static_cast<int>(layer)][static_cast<int>(kind)] = false;
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

	ParamValue get(const std::string name) override { return 0.f; }
	void set(const std::string name, ParamValue value) override {}

	ParamValue get(const std::string name, int index) override {
		if (name == "nDims") {
			return nDims[index];
		} else if (name == "range") {
			return range[index];
		} else if (name == "layer_element") {
			return layerElement[index];
		} else if (name == "hover_color") {
			return hoverColor;
		} else if (name == "select_color") {
			return selectColor;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, int index, ParamValue value) override {
		if (name == "nDims") {
			if (auto* pVal = std::get_if<int>(&value))
				nDims[index] = *pVal;
		} else if (name == "range") {
			if (auto* pVal = std::get_if<glm::vec2>(&value))
				range[index] = *pVal;
		} else if (name == "layer_element") {
			if (auto* pVal = std::get_if<int>(&value))
				layerElement[index] = *pVal;
		} else if (name == "hover_color") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				hoverColor = *pVal;
		} else if (name == "select_color") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				selectColor = *pVal;
		}
	}

	void setColormapTexture(int i, Colormap &colormap) {
		texColormaps[i] = colormap.tex;
	}

	void loadState(json &j) {

		for (int i = 0; i < 5; ++i) {
			nDims[i] = j["n_dims"][i].get<int>();
			range[i] = {j["range"][i][0].get<float>(), j["range"][i][1].get<float>()};
			layerElement[i] = j["layer_element"][i].get<int>();
		}

		hoverColor = {j["hover_color"][0].get<float>(), j["hover_color"][1].get<float>(), j["hover_color"][2].get<float>()};
		selectColor = {j["select_color"][0].get<float>(), j["select_color"][1].get<float>(), j["select_color"][2].get<float>()};
	}

	void saveState(json &j) const {

		for (int i = 0; i < 5; ++i) {
			j["n_dims"][i] = nDims[i];
			j["range"][i] = {range[i][0], range[i][1]};
			j["layer_element"][i] = layerElement[i];

			for (int k = 0; k < 7; ++k)
				j["activated_layers"][i][k] = activatedLayers[i][k];
		}

		j["hover_color"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["select_color"] = json::array({selectColor.x, selectColor.y, selectColor.z});
	}

	int nDims[5];
	glm::vec2 range[5];
	int layerElement[5] = {-1, -1, -1, -1, -1}; // TODO to remove replaced by activatedLayers
	bool activatedLayers[5][7] = {false};
	glm::vec3 hoverColor{1.f, 1.f, 1.f};
	glm::vec3 selectColor{0.f, 0.22f, 1.f};

	private:
	unsigned int texColormaps[3]; // Colormap textures
	unsigned int bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures
};