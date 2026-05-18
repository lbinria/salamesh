#pragma once
#include "material_params.h"
#include "../data/layer.h"
#include "../utils/opengl_helper.h"

struct LayersParams : MaterialParams {

	void init() override {
		sl::createTBO(bufHighlight, tboHighlight);
		sl::createTBO(bufFilter, tboFilter);
		sl::createTBO(bufColormap0, tboColormap0);
		sl::createTBO(bufColormap1, tboColormap1);
		sl::createTBO(bufColormap2, tboColormap2);
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

		}

		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);
		shader.setInt("highlightBuf", 3);
		shader.setInt("filterBuf", 4);
		shader.setInt("colormap0Buf", 5);
		shader.setInt("colormap1Buf", 6);
		shader.setInt("colormap2Buf", 7);

		// glActiveTexture(GL_TEXTURE0 + 0);
		// glBindTexture(GL_TEXTURE_2D, texColormap0);

		// glActiveTexture(GL_TEXTURE0 + 1);
		// glBindTexture(GL_TEXTURE_2D, texColormap1);

		// glActiveTexture(GL_TEXTURE0 + 2);
		// glBindTexture(GL_TEXTURE_2D, texColormap2);

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

	void setLayerElement(int element, Layer layer) {
		layerElement[static_cast<int>(layer)] = element;
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
		}
	}

	void loadState(json &j) {
		// TODO implement
			// size = j["pointSize"].get<float>();
			// color = {j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()};
	}

	void saveState(json &j) const {
		// TODO implement
		// j["pointSize"] = size;
		// j["pointColor"] = json::array({color.x, color.y, color.z});
	}

	int nDims[5];
	glm::vec2 range[5];
	int layerElement[5] = {-1, -1, -1, -1, -1};

	private:
	unsigned int bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures
};