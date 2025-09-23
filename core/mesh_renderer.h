// #pragma once

// #include "color_mode.h"
// #include "element.h"
// #include "shader.h"
// #include "attribute.h"
// #include "../include/glm/glm.hpp"


// struct MeshRenderer : public IRenderer {


// 	void setAttrElement(int element) {
// 		shader.use();
// 		shader.setInt("attrElement", element);
// 	}

// 	void setHighlightElement(ElementKind element) {
// 		shader.use();
// 		shader.setInt("highlightElement", element);
// 	}

// 	void setFilterElement(ElementKind element) {
// 		shader.use();
// 		shader.setInt("filterElement", element);
// 	}

// 	// Only on mesh
// 	float getMeshSize() const {
// 		return meshSize;
// 	}

// 	void setMeshSize(float val) {
// 		shader.use();
// 		shader.setFloat("meshSize", val);
// 		meshSize = val;
// 	}

// 	// Only on mesh
// 	float getMeshShrink() const {
// 		return meshShrink;
// 	}

// 	void setMeshShrink(float val) {
// 		shader.use();
// 		shader.setFloat("meshShrink", val);
// 		meshShrink = val;
// 	}

// 	// Only on mesh
// 	void setMeshIndex(int index) {
// 		shader.use();
// 		shader.setInt("meshIndex", index);
// 	}


// 	void setAttribute(ContainerBase *ga, int element) {
// 		// Set attribute element to shader
// 		shader.use();
// 		shader.setInt("attrElement", element);

// 		// Prepare data
// 		std::vector<float> converted_attribute_data;

// 		// Transform data
// 		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

// 			converted_attribute_data.resize(a->data.size());
// 			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

// 		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			
// 			converted_attribute_data.resize(a->data.size());
// 			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

// 		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			
// 			converted_attribute_data.resize(a->data.size());
// 			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

// 		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {

// 			converted_attribute_data.resize(a->data.size());
// 			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

// 		}

// 		// Set attribute data to shader
// 		setAttribute(converted_attribute_data);
// 	}

// 	void setAttribute(std::vector<float> data) {

// 			// Get bounds (min-max)
// 		float min = std::numeric_limits<float>::max(); 
// 		float max = std::numeric_limits<float>::min();
// 		for (auto x : data) {
// 			min = std::min(min, x);
// 			max = std::max(max, x);
// 		}

// 		// Update min/max
// 		shader.use();
// 		shader.setFloat2("attrRange", glm::vec2(min, max));

// 		// Update sample
// 		std::memcpy(ptrAttr, data.data(), data.size() * sizeof(float));
// 	}

// 	// Maybe protected ?
// 	void setHighlight(int idx, float highlight) {
// 		// ptrHighlight[idx] = highlight;
// 		highlightBuffer.ptr[idx] = highlight;
// 	}

// 	// Maybe protected ?
// 	void setHighlight(std::vector<float> highlights) {
// 		// std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
// 		std::memcpy(highlightBuffer.ptr, highlights.data(), highlights.size() * sizeof(float));
// 	}

// 	// Maybe protected ?
// 	void setFilter(int idx, bool filter) {
// 		ptrFilter[idx] = filter ? 1.f : 0.f;
// 	}

// 	// TODO not tested !
// 	// Maybe protected ?
// 	void setFilter(std::vector<bool> filters) {
// 		std::vector<float> f_filters(filters.size());
// 		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
// 		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
// 	}

// 	float* &getFilterPtr() {
// 		return ptrFilter;
// 	}

// 	Shader shader;

// 	protected:

// 	bool visible = true;

//     glm::vec3 color{0.8f, 0.f, 0.2f};
//     float meshSize = 0.01f;
//     float meshShrink = 0.f;

// 	unsigned int VAO, VBO; // Buffers
// 	unsigned int bufBary, bufHighlight, bufAttr, bufFilter; // Sample buffers
// 	unsigned int texColorMap, texBary, texHighlight, texHighlight2, texAttr, texFilter; // Textures

// 	float *ptrAttr;
// 	float *ptrHighlight;
// 	float *ptrFilter;


// 	struct PersistentBuffer {
// 		GLuint buf = 0;
// 		float* ptr = nullptr;
// 		GLsizeiptr size = 0;
// 		GLbitfield mapFlags = 0;
// 		GLbitfield storageFlags = 0;
// 	};

// 	PersistentBuffer highlightBuffer;

// 	void createPersistentBuffer(PersistentBuffer &b, GLsizeiptr size,
// 								GLbitfield mapFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
// 								GLbitfield storageFlags = GL_DYNAMIC_STORAGE_BIT) {
// 		if (b.buf) {
// 			glDeleteBuffers(1, &b.buf);
// 			b.buf = 0;
// 		}
// 		b.size = size;
// 		b.mapFlags = mapFlags;
// 		b.storageFlags = storageFlags | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT; // ensure write/persistent for mapping

// 		glGenBuffers(1, &b.buf);
// 		glBindBuffer(GL_TEXTURE_BUFFER, b.buf);
// 		glBufferStorage(GL_TEXTURE_BUFFER, size, nullptr, mapFlags);
// 		// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
// 		b.ptr = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, size, mapFlags);

// 		// glCreateBuffers(1, &b.buf);
// 		// glNamedBufferStorage(b.buf, size, nullptr, b.storageFlags);
// 		// b.ptr = glMapNamedBufferRange(b.buf, 0, size, b.mapFlags);
// 		if (!b.ptr) throw std::runtime_error("Failed to map buffer");
// 	}

// 	void destroyPersistentBuffer(PersistentBuffer &b) {
// 		if (b.buf == 0) return;
// 		if (b.ptr) {
// 			glUnmapNamedBuffer(b.buf);
// 			b.ptr = nullptr;
// 		}
// 		glDeleteBuffers(1, &b.buf);
// 		b.buf = 0;
// 		b.size = 0;
// 	}

// 	void resizePersistentBuffer(PersistentBuffer &b, GLsizeiptr newSize)
// 	{
// 		if (newSize == b.size) return;
// 		// Create new buffer
// 		PersistentBuffer nb;
// 		createPersistentBuffer(nb, newSize, b.mapFlags, b.storageFlags);
// 		// Copy existing data (up to min size)
// 		if (b.ptr && nb.ptr) {
// 			GLsizeiptr copySize = (b.size < newSize) ? b.size : newSize;
// 			std::memcpy(nb.ptr, b.ptr, (size_t)copySize);
// 		} else {
// 			// Fallback to glCopyNamedBufferSubData if you prefer GPU-side copy:
// 			// glCopyNamedBufferSubData(b.buf, nb.buf, 0, 0, std::min(b.size, newSize));
// 		}
// 		// Replace old buffer with new
// 		destroyPersistentBuffer(b);
// 		b = nb; // move new buffer into b
// 	}


// 	int nverts = 0;

// };