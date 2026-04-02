#pragma once 
#include "../shader.h"
#include "../graphic_api.h"
#include "../element.h"
#include "../layer.h"
#include "../helpers.h"
#include "../../include/json.hpp"
using json = nlohmann::json;

#include <vector>

struct RendererView {

	RendererView(Shader shader) : 
		shader(std::move(shader))
	{
		init();
	}

	void init() {
		// For the moment don't use persistent mapped memory
		// TODO clean DO THAT IN RENDERER ?
		sl::createTBO(bufHighlight, tboHighlight);
		sl::createTBO(bufFilter, tboFilter);
		sl::createTBO(bufColormap0, tboColormap0);
		sl::createTBO(bufColormap1, tboColormap1);
		sl::createTBO(bufColormap2, tboColormap2);

		shader.use();
		shader.setInt("colormap0", 0);
		shader.setInt("colormap1", 1);
		shader.setInt("colormap2", 2);
		shader.setInt("highlightBuf", 3);
		shader.setInt("filterBuf", 4);
		shader.setInt("colormap0Buf", 5);
		shader.setInt("colormap1Buf", 6);
		shader.setInt("colormap2Buf", 7);
	}

	virtual int getRenderElementKind() = 0;

	bool isRenderElement(ElementKind kind) {
		return (getRenderElementKind() & kind) == kind;
	}

	void use(glm::vec3 &position) {
		
		useTextures();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);

		shader.use();
		shader.setMat4("model", model);
	}

	virtual void useTextures() {};

	void setColormap0Texture(unsigned int tex) { texColormap0 = tex; }
	void setColormap1Texture(unsigned int tex) { texColormap1 = tex; }
	void setColormap2Texture(unsigned int tex) { texColormap2 = tex; }

	void setLightEnabled(bool enabled) {
		shader.use();
		shader.setFloat("is_light_enabled", enabled); // TODO set int here !
	}

	void setLayerElement(int element, Layer layer) {
		// I could refactor more, 
		// but we won't understand anything after that
		switch (layer)
		{
		case Layer::COLORMAP_0:
		case Layer::COLORMAP_1:
		case Layer::COLORMAP_2:
			shader.use();
			shader.setInt("colormapElement[" + std::to_string(int(layer)) + "]", element);
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

	void setLayerNDims(Layer layer, int nDims) {
		// TODO check layer ? only works for colormaps...
		shader.use();
		shader.setInt("attrNDims[" + std::to_string(int(layer)) + "]", nDims);
	}

	void setLayerRange(Layer layer, float min, float max) {
		// TODO check layer ? only works for colormaps...
		shader.use();
		shader.setFloat2("attrRange[" + std::to_string(int(layer)) + "]", glm::vec2(min, max));
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

	void clean() {
		glDeleteBuffers(1, &bufHighlight);
		glDeleteTextures(1, &tboHighlight);
		glDeleteBuffers(1, &bufFilter);
		glDeleteTextures(1, &tboFilter);
		glDeleteBuffers(1, &bufColormap0);
		glDeleteTextures(1, &tboColormap0);
		glDeleteBuffers(1, &bufColormap1);
		glDeleteTextures(1, &tboColormap1);
		glDeleteBuffers(1, &bufColormap2);
		glDeleteTextures(1, &tboColormap2);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		// Clean shader
		shader.clean();
	}

	bool visible;

	void loadState(json &j) {
		visible = j["visible"].get<bool>();
		doLoadState(j);
	}

	void saveState(json &j) const {
		j["visible"] = visible;
		doSaveState(j);
	}



	protected:
	Shader shader;

	// TODO move this to a RendererView subclass (LayeredRendererView for example)
	unsigned int bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int texColormap0, texColormap1, texColormap2, tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures

	// virtual void doLoadState(json &j) = 0;
	// virtual void doSaveState(json &j) const = 0;
	// TODO uncomment above and remove below
	virtual void doLoadState(json &j) {};
	virtual void doSaveState(json &j) const {};


};

struct LayeredRendererView : public RendererView {

};

struct MeshRendererView : public RendererView {
	
	using RendererView::RendererView;

	float getMeshSize() const {
		return meshSize;
	}

	void setMeshSize(float val) {
		shader.use();
		shader.setFloat("meshSize", val);
		meshSize = val;
	}

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

	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
	}

	virtual void useTextures() override {
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texColormap0);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texColormap1);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, texColormap2);

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

	virtual void doLoadState(json &j) override {
		setColor(glm::vec3(j["color"][0].get<float>(), j["color"][1].get<float>(), j["color"][2].get<float>()));
		setMeshSize(j["meshSize"].get<float>());
		setMeshShrink(j["meshShrink"].get<float>());
		setCornerVisible(j["isCornerVisible"].get<bool>());
	}

	virtual void doSaveState(json &j) const override {
		j["color"] = json::array({color.x, color.y, color.z});
		j["meshSize"] = meshSize;
		j["meshShrink"] = meshShrink;
		j["isCornerVisible"] = isCornerVisible;
	}

	private:

	float meshSize;
	float meshShrink;
	bool isCornerVisible;
	glm::vec3 color;

};

struct ClippingRendererView : public RendererView {
	ClippingRendererView() : 
		RendererView(Shader(sl::shadersPath("clipping.vert"), sl::shadersPath("clipping.frag"))) 
	{}

	int getRenderElementKind() override { return 0; }

};

struct HalfedgeRendererView : public MeshRendererView {
	HalfedgeRendererView() : 
		MeshRendererView(Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))) 
	{}

	int getRenderElementKind() override { return ElementKind::EDGES_ELT | ElementKind::CORNERS_ELT; }

};

struct VolumeRendererView : public MeshRendererView {
	VolumeRendererView() : 
		MeshRendererView(Shader(sl::shadersPath("volume.vert"), sl::shadersPath("volume.frag"))) 
	{}

	int getRenderElementKind() override { return ElementKind::CELLS_ELT | ElementKind::CELL_FACETS_ELT; }

};

struct SurfaceRendererView : public MeshRendererView {
	SurfaceRendererView() : 
		MeshRendererView(Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))) 
	{}

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }

};

struct LineRendererView : public RendererView {
	LineRendererView() : 
		RendererView(Shader(sl::shadersPath("gizmo_line.vert"), sl::shadersPath("gizmo_line.frag"))) 
	{}

	int getRenderElementKind() override { return 0; }

};

struct BBoxRendererView : public RendererView {
	BBoxRendererView() : 
		RendererView(Shader(sl::shadersPath("bbox.vert"), sl::shadersPath("bbox.frag"))) 
	{}

	int getRenderElementKind() override { return 0; }


	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
	}

	void doLoadState(json &j) override {
		setColor({j["bboxColor"][0].get<float>(), j["bboxColor"][1].get<float>(), j["bboxColor"][2].get<float>()});
	}

	void doSaveState(json &j) const override {
		j["bboxColor"] = json::array({color.x, color.y, color.z});
	}

	private:
	glm::vec3 color;
};

struct PolyRendererView : public MeshRendererView {
	PolyRendererView() : 
		MeshRendererView(Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))) 
	{}

	void setNVertsPerFacetBuf(unsigned int texNumber, unsigned int texId) {
		glActiveTexture(GL_TEXTURE0 + texNumber);
		glBindTexture(GL_TEXTURE_BUFFER, texId);
		// TODO Make cache to not set shader everytime
		shader.use();
		shader.setInt("nvertsPerFacetBuf", texNumber);
	}

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }

};


struct PointSetRendererView : public RendererView {

	PointSetRendererView() : 
		RendererView(Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))) 
	{}

	int getRenderElementKind() override { return ElementKind::POINTS_ELT; }


	float getPointSize() const {
		return pointSize;
	}

	void setPointSize(float size) {
		shader.use();
		shader.setFloat("pointSize", size);
		pointSize = size;
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("pointColor", c);
		color = c;
	}

	void useTextures() override {
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texColormap0);

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, texColormap1);

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, texColormap2);

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

    void doLoadState(json &j) override {
        setPointSize(j["pointSize"].get<float>());
        setColor({j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()});
    }

    void doSaveState(json &j) const override {
        j["pointSize"] = pointSize;
        j["pointColor"] = json::array({color.x, color.y, color.z});
    }

	private:
	float pointSize;
	glm::vec3 color;
};