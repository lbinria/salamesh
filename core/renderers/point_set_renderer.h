#pragma once

#include <vector>
#include <optional>

#include "../include/json.hpp"
using json = nlohmann::json;

#include <ultimaille/all.h>

#include "renderer.h"

#include "../shader.h"

using namespace UM;

struct PointSetRenderer : public Renderer {

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
	};

	PointSetRenderer(std::string name, PointSet &ps) : 
		Renderer(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))),
		ps(ps) {}

	PointSetRenderer(std::string name) : 
		Renderer(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))), ps(*new PointSet()) {}

	RenderPrimitive getRenderPrimitive() const override {
		return RenderPrimitive::RENDER_POINTS;
	}

	void clear() override;

	const void * getData() override;
	MaterialInstance getDefaultMaterial() override;

	size_t getElementSize() override {
		return sizeof(Vertex);
	}

	std::vector<RendererElementField> getElementFields() override {
		return {
			{
				.name = "vertexIndex",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
				.offset = static_cast<int>(offsetof(Vertex, vertexIndex))
			},
			{
				.name = "aPos",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
				.offset = static_cast<int>(offsetof(Vertex, position))
			},
			{
				.name = "sizeScale",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_FLOAT,
				.offset = static_cast<int>(offsetof(Vertex, size))
			}
		};
	}




	int addPoint(glm::vec3 p) {
		int off = ps.create_points(1);
		ps[off] = sl::glm2um(p);

		if (autoUpdate)
			requestUpdate();

		return off;
	}

	int addPoints(std::vector<glm::vec3> points) {
		int off = ps.create_points(points.size());
		for (int i = off; i < points.size() - 1; ++i)
			ps[i] = sl::glm2um(points[i]);

		if (autoUpdate)
			requestUpdate();

		return off;
	}

	// TODO add removePoint, and removePoitns by index not bool vector
	void removePoints(std::vector<bool> toKill) {
		ps.delete_points(toKill);

		if (autoUpdate)
			requestUpdate();
	}

	void clearPoints() {
		std::vector<bool> toKill(ps.size(), true);
		ps.delete_points(toKill);

		if (autoUpdate)
			requestUpdate();
	}

	inline int count() {
		return ps.size();
	}



	bool getAutoUpdate() { return autoUpdate; }
	void setAutoUpdate(bool val) { autoUpdate = val; }


	PointSet& getPointSet() { return ps; }

	vec3& operator[](int index)
	{
		return ps[index];
	}


    private:
    PointSet &ps;
	bool autoUpdate = false;

    float pointSize;
    glm::vec3 color;

	// TODO to remove
    void doLoadState(json &j) override {}
    void doSaveState(json &j) const override {}

};