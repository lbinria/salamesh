#pragma once

#include "../color_mode.h"
#include "../element.h"
#include "../layer.h"

#include "../shader.h"
#include "../attribute.h"
#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../helpers.h"

struct IRenderer {

	enum ClippingMode {
		CELL = 0,
		STD = 1
	};


	// User must overwrite which element(s) are supported for rendering
	virtual int getRenderElementKind() = 0;

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;

	bool isRenderElement(ElementKind kind) {
		return (getRenderElementKind() & kind) == kind;
	}

	void setTexture(unsigned int tex) { texColorMap = tex; }

	void setColormap0Texture(unsigned int tex) { texColormap0 = tex; }
	void setColormap1Texture(unsigned int tex) { texColormap1 = tex; }
	void setColormap2Texture(unsigned int tex) { texColormap2 = tex; }

	void setVisible(bool v) {
		visible = v;
	}

	bool getVisible() const {
		return visible;
	}

	IRenderer(Shader shader) : shader(std::move(shader)) {}

	// TODO see list of shader function, some are specific to mesh_renderer...
	void setModel(glm::mat4 model) {
		shader.use();
		shader.setMat4("model", model);
	}

	void setAttrElement(int element) {
		shader.use();
		shader.setInt("attrElement", element);
	}

	int getAttrRepeat() {
		return attrRepeat;
	}

	void setAttrRepeat(int n) {
		attrRepeat = n <= 0 ? 1 : n;
		shader.use();
		shader.setInt("attrRepeat", attrRepeat);
	}

	void setLayerElement(int element, Layer layer) {
		switch (layer)
		{
		case Layer::COLORMAP_0:
			shader.use();
			shader.setInt("colormapElement0", element);
			break;
		case Layer::COLORMAP_1:
			shader.use();
			shader.setInt("colormapElement1", element);
			break;
		case Layer::COLORMAP_2:
			shader.use();
			shader.setInt("colormapElement2", element);
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

	void setColorMode(ColorMode mode) {
		shader.use();
		shader.setInt("colorMode", mode);
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
	}

	glm::vec3 getHoverColor() const {
		return hoverColor;
	}

	void setHoverColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("hoverColor", c);
		hoverColor = c;
	}

	glm::vec3 getSelectColor() const {
		return selectColor;
	}

	void setSelectColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("selectColor", c);
		selectColor = c;
	}

	void setLight(bool enabled) {
		shader.use();
		shader.setFloat("is_light_enabled", enabled); // TODO set int here !
	}

	void setLightFollowView(bool follow) {
		shader.use();
		shader.setInt("is_light_follow_view", follow);
	}

	virtual void setClippingMode(ClippingMode mode) {
		shader.use();
		shader.setInt("clipping_mode", mode);		
	}

	virtual void setClipping(bool enabled) {
		shader.use();
		shader.setInt("is_clipping_enabled", enabled);
	}

	virtual void setClippingPlanePoint(glm::vec3 p) {
		shader.use();
		shader.setFloat3("clipping_plane_point", p);
	}

	virtual void setClippingPlaneNormal(glm::vec3 n) {
		shader.use();
		shader.setFloat3("clipping_plane_normal", n);
	}

	void setInvertClipping(bool invert) {
		shader.use();
		shader.setInt("invert_clipping", invert);
	}

	// Only on mesh
	float getMeshSize() const {
		return meshSize;
	}

	void setMeshSize(float val) {
		shader.use();
		shader.setFloat("meshSize", val);
		meshSize = val;
	}

	// Only on mesh
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

	// void setSelectedColormap(int idx) {
		// shader.use();
		// shader.setInt("colormap", idx);
	// }

	// Only on mesh
	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}

	// TODO make private
	template<typename T, int nDims>
	void map(std::vector<T>& data, int selectedDim, std::vector<float>& result) {

		if constexpr (nDims == 1) {
			result.resize(data.size());
			for (int i = 0; i < data.size(); ++i) {
				result[i] = static_cast<float>(data[i]);
			}
		} else {
			// YOu can refactor this !
			if (selectedDim == -1) {
				// Inline data
				result.resize(data.size() * nDims);
				for (int i = 0; i < data.size(); ++i) {
					for (int d = 0; d < nDims; ++d) {
						result[i * nDims + d] = static_cast<float>(data[i][d]);
					}
				}
			} else {
				result.resize(data.size());
				for (int i = 0; i < data.size(); ++i) {
					result[i] = static_cast<float>(data[i][selectedDim]);
				}
			}
		}
	}

	void setAttribute(Attribute& attr) {

		ContainerBase *ga = attr.ptr.get();

		// Prepare data
		std::vector<float> data;

		// Transform data
		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {
			map<double, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			map<float, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			map<int, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {
			map<bool, 1>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(ga)) {
			map<vec2, 2>(a->data, attr.selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(ga)) {
			map<vec3, 3>(a->data, attr.selectedDim, data);
		} else {
			throw std::runtime_error("Attribute type is not supported in `Renderer::setAttribute`.");
		}

		// Update attribute dimensions
		shader.use();
		shader.setInt("attrNDims", attr.getDims());

		// Get range (min-max)
		auto [min, max] = sl::getRange(data);

		// Update min/max
		shader.use();
		shader.setFloat2("attrRange", glm::vec2(min, max));

		// for (int d = 0; d < attr.getDims(); ++d) {
		// 	auto [min, max] = getRange(data, d, attr.getDims());
		// 	// Update min/max
		// 	shader.use();
		// 	shader.setFloat2AtIndex("attrRanges", d, glm::vec2(min, max));
		// }

		// Update buffer
		glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}


	void setLayerRange(Layer layer, float min, float max) {
		// Update min/max
		shader.use();
		shader.setFloat2("attrRange" + std::to_string(int(layer)), glm::vec2(min, max));
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

	void loadState(json &j) {
		setVisible(j["visible"].get<bool>());
		setColor(glm::vec3(j["color"][0].get<float>(), j["color"][1].get<float>(), j["color"][2].get<float>()));
		setHoverColor(glm::vec3(j["hoverColor"][0].get<float>(), j["hoverColor"][1].get<float>(), j["hoverColor"][2].get<float>()));
		setSelectColor(glm::vec3(j["selectColor"][0].get<float>(), j["selectColor"][1].get<float>(), j["selectColor"][2].get<float>()));
		setMeshSize(j["meshSize"].get<float>());
		setMeshShrink(j["meshShrink"].get<float>());
		doLoadState(j);
	}

	void saveState(json &j) const {
		j["visible"] = visible;
		j["color"] = json::array({color.x, color.y, color.z});
		j["hoverColor"] = json::array({hoverColor.x, hoverColor.y, hoverColor.z});
		j["selectColor"] = json::array({selectColor.x, selectColor.y, selectColor.z});
		j["meshSize"] = meshSize;
		j["meshShrink"] = meshShrink;
		doSaveState(j);
	}



	Shader shader;

	protected:

	bool visible = true;

	glm::vec3 color{0.71f, 0.71f, 0.71f};
	glm::vec3 hoverColor{1.f, 1.f, 1.f};
	glm::vec3 selectColor{0.f, 0.22f, 1.f};
	float meshSize = 0.01f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
	int attrRepeat = 1;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufAttr, bufColormap0, bufColormap1, bufColormap2, bufHighlight, bufFilter; // Sample buffers
	unsigned int texAttr, texColorMap, texColormap0, texColormap1, texColormap2, tboColormap0, tboColormap1, tboColormap2, tboHighlight, tboFilter; // Textures

	float *ptrAttr;
	// float *ptrHighlight;
	// float *ptrFilter;


	struct PersistentBuffer {
		GLuint buf = 0;
		float* ptr = nullptr;
		GLsizeiptr size = 0;
		GLbitfield mapFlags = 0;
		GLbitfield storageFlags = 0;
	};

	// PersistentBuffer highlightBuffer;

	void createPersistentBuffer(PersistentBuffer &b, GLsizeiptr size,
								GLbitfield mapFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
								GLbitfield storageFlags = GL_DYNAMIC_STORAGE_BIT) {
		if (b.buf) {
			glDeleteBuffers(1, &b.buf);
			b.buf = 0;
		}
		b.size = size;
		b.mapFlags = mapFlags;
		b.storageFlags = storageFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT; // ensure write/persistent for mapping

		glGenBuffers(1, &b.buf);
		glBindBuffer(GL_TEXTURE_BUFFER, b.buf);
		glBufferStorage(GL_TEXTURE_BUFFER, size, nullptr, mapFlags);
		// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
		b.ptr = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, size, mapFlags);

		// glCreateBuffers(1, &b.buf);
		// glNamedBufferStorage(b.buf, size, nullptr, b.storageFlags);
		// b.ptr = glMapNamedBufferRange(b.buf, 0, size, b.mapFlags);
		if (!b.ptr) throw std::runtime_error("Failed to map buffer");
	}

	void destroyPersistentBuffer(PersistentBuffer &b) {
		if (b.buf == 0) return;
		if (b.ptr) {
			glUnmapNamedBuffer(b.buf);
			b.ptr = nullptr;
		}
		glDeleteBuffers(1, &b.buf);
		b.buf = 0;
		b.size = 0;
	}

	void resizePersistentBuffer(PersistentBuffer &b, GLsizeiptr newSize)
	{
		if (newSize == b.size) return;
		// Create new buffer
		PersistentBuffer nb;
		createPersistentBuffer(nb, newSize, b.mapFlags, b.storageFlags);
		// Copy existing data (up to min size)
		if (b.ptr && nb.ptr) {
			GLsizeiptr copySize = (b.size < newSize) ? b.size : newSize;
			std::memcpy(nb.ptr, b.ptr, (size_t)copySize);
		} else {
			// Fallback to glCopyNamedBufferSubData if you prefer GPU-side copy:
			// glCopyNamedBufferSubData(b.buf, nb.buf, 0, 0, std::min(b.size, newSize));
		}
		// Replace old buffer with new
		destroyPersistentBuffer(b);
		b = nb; // move new buffer into b
	}


	int nverts = 0;

	private:

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;

};