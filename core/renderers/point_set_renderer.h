#pragma once

#include <vector>
#include <optional>

#include "../include/json.hpp"
using json = nlohmann::json;

#include <ultimaille/all.h>

#include "renderer.h"

#include "../shader.h"

using namespace UM;

struct PointSetRenderer : public IRenderer {

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
	};

	PointSetRenderer(PointSet &ps) : 
		IRenderer(Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))),
		ps(ps) {
			setPointSize(4.0f); // TODO here use a setting default point size
			setColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
		}

	PointSetRenderer() : 
		IRenderer(Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))), ps(*new PointSet()) {
			setPointSize(4.0f);
			setColor({0.23, 0.85, 0.66});
		}

	void init() override;
	void push() override;
	void render(glm::vec3 &position) override;
	void clean() override;
	void clear() override;

	int getRenderElementKind() override { return ElementKind::POINTS_ELT; }

	void addPoint(glm::vec3 p) {
		int off = ps.create_points(1);
		ps[off] = sl::glm2um(p);

		if (autoUpdate)
			push();
	}

	void addPoints(std::vector<glm::vec3> points) {
		int off = ps.create_points(points.size());
		for (int i = off; i < points.size() - 1; ++i)
			ps[i] = sl::glm2um(points[i]);

		if (autoUpdate)
			push();
	}

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

    void doLoadState(json &j) override {
        setPointSize(j["pointSize"].get<float>());
        setColor({j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()});
    }

    void doSaveState(json &j) const override {
        j["pointSize"] = pointSize;
        j["pointColor"] = json::array({color.x, color.y, color.z});
    }

};