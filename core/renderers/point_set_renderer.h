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

	void init() override;
	void push() override;
	void render(RendererView &rv, glm::vec3 &position) override;
	void clean() override;
	void clear() override;

	std::unique_ptr<RendererView> getDefaultView() override { 
		auto rv = std::make_unique<PointSetRendererView>(); 
		rv->setPointSize(4.0f);
		rv->setColor({0.23, 0.85, 0.66});
		return rv;
	}

	int addPoint(glm::vec3 p) {
		int off = ps.create_points(1);
		ps[off] = sl::glm2um(p);

		if (autoUpdate)
			push();

		return off;
	}

	int addPoints(std::vector<glm::vec3> points) {
		int off = ps.create_points(points.size());
		for (int i = off; i < points.size() - 1; ++i)
			ps[i] = sl::glm2um(points[i]);

		if (autoUpdate)
			push();

		return off;
	}

	// TODO add removePoint, and removePoitns by index not bool vector
	void removePoints(std::vector<bool> toKill) {
		ps.delete_points(toKill);

		if (autoUpdate)
			push();
	}

	void clearPoints() {
		std::vector<bool> toKill(ps.size(), true);
		ps.delete_points(toKill);

		if (autoUpdate)
			push();
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