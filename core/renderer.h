#pragma once

#include "color_mode.h"
#include "element.h"
#include "shader.h"
#include "attribute.h"
#include "../include/glm/glm.hpp"


struct IRenderer {

	enum Layer {
		COLORMAP,
		HIGHLIGHT,
		FILTER
	};

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;

	void setTexture(unsigned int tex) { texColorMap = tex; }

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

	void setAttrRange(glm::vec2 range) {
		shader.use();
		shader.setFloat2("attrRange", range);
	}

	void setHighlightElement(ElementKind element) {
		shader.use();
		shader.setInt("highlightElement", element);
	}

	void setFilterElement(ElementKind element) {
		shader.use();
		shader.setInt("filterElement", element);
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

	void setLight(bool enabled) {
		shader.use();
		shader.setFloat("is_light_enabled", enabled);
	}

	void setLightFollowView(bool follow) {
		shader.use();
		shader.setInt("is_light_follow_view", follow);
	}

	void setClipping(bool enabled) {
		shader.use();
		shader.setInt("is_clipping_enabled", enabled);
	}

	void setClippingPlanePoint(glm::vec3 p) {
		shader.use();
		shader.setFloat3("clipping_plane_point", p);
	}

	void setClippingPlaneNormal(glm::vec3 n) {
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

	void setSelectedColormap(int idx) {
		shader.use();
		shader.setInt("colormap", idx);
	}

	// Only on mesh
	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}


	void setAttribute(ContainerBase *ga, int element) {
		// Set attribute element to shader
		shader.use();
		shader.setInt("attrElement", element);

		// Prepare data
		std::vector<float> converted_attribute_data;

		// Transform data
		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			
			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			
			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {

			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		}

		// Set attribute data to shader
		setAttribute(converted_attribute_data);
	}

	std::tuple<float, float> getRange(std::vector<float>& data) {
		float min = std::numeric_limits<float>::max(); 
		float max = std::numeric_limits<float>::min();
		for (auto x : data) {
			min = std::min(min, x);
			max = std::max(max, x);
		}

		return std::make_tuple(min, max);
	}

	void setAttribute(std::vector<float> data) {

		// Get range (min-max)
		auto [min, max] = getRange(data);

		// Update min/max
		shader.use();
		shader.setFloat2("attrRange", glm::vec2(min, max));

		// Update sample
		std::memcpy(ptrAttr, data.data(), data.size() * sizeof(float));
	}

	void setHighlight(int idx, float val) {
		// ptrHighlight[idx] = highlight;
		// highlightBuffer.ptr[idx] = highlight;
		glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
	}

	void setHighlight(std::vector<float> data) {
		// std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
		// std::memcpy(highlightBuffer.ptr, highlights.data(), highlights.size() * sizeof(float));
		glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}

	void setFilter(int idx, float val) {
		// ptrFilter[idx] = filter ? 1.f : 0.f;
		glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
		glBufferSubData(GL_TEXTURE_BUFFER, idx * sizeof(float), sizeof(float), &val);
	}

	// TODO not tested !
	// Maybe protected ?
	void setFilter(std::vector<float> data) {
		// std::vector<float> f_filters(filters.size());
		// std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
		// std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
		glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
		glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);
	}

	
	void setLayer(int idx, float val, Layer layer) {
		switch (layer)
		{
		case Layer::HIGHLIGHT:
			setHighlight(idx, val);
			break;
		case Layer::FILTER:
			setFilter(idx, val);
			break;
		default:
			break;
		}
	}

	void setLayer(std::vector<float> data, Layer layer) {
		switch (layer)
		{
		case Layer::HIGHLIGHT:
			setHighlight(data);
			break;
		case Layer::FILTER:
			setFilter(data);
			break;
		default:
			break;
		}
	}

	float* &getFilterPtr() {
		return ptrFilter;
	}

	// void resizeHightlightBuffer(int size) {		
	// 	resizePersistentBuffer(highlightBuffer, size);
	// 	// unsigned int newTexHighlight = 0;
	// 	// glCreateTextures(GL_TEXTURE_BUFFER, 1, &newTexHighlight);
	// 	// glActiveTexture(GL_TEXTURE0 + 3); 
	// 	// glTextureBuffer(newTexHighlight, GL_R32F, highlightBuffer.buf);
		
	// 	// glActiveTexture(GL_TEXTURE0 + 3);
	// 	// glBindTexture(GL_TEXTURE_BUFFER, newTexHighlight);

	// 	// shader.use();
	// 	// shader.setInt("highlightBuf", 3);

	// 	// glDeleteTextures(1, &texHighlight2);
	// 	// texHighlight2 = texHighlight;
	// }

	Shader shader;

	protected:

	bool visible = true;

    glm::vec3 color{0.8f, 0.f, 0.2f};
    float meshSize = 0.01f;
    float meshShrink = 0.f;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufBary, bufHighlight, bufAttr, bufFilter; // Sample buffers
	unsigned int texColorMap, texBary, texHighlight, texAttr, texFilter; // Textures

	float *ptrAttr;
	// float *ptrHighlight;
	float *ptrFilter;


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

};