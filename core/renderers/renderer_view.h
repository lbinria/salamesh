#pragma once 
#include "../shader.h"

struct RendererView {

	RendererView(Shader shader) : 
		shader(std::move(shader))
	{}

	void use(glm::vec3 &position) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);

		shader.use();
		shader.setMat4("model", model);
	}

	bool visible;

	protected:
	Shader shader;

	// virtual void doLoadState(json &j) = 0;
	// virtual void doSaveState(json &j) const = 0;
	// TODO uncomment above and remove below
	virtual void doLoadState(json &j) {};
	virtual void doSaveState(json &j) const {};

};

struct ClippingRendererView : public RendererView {
	ClippingRendererView() : 
		RendererView(Shader(sl::shadersPath("clipping.vert"), sl::shadersPath("clipping.frag"))) 
	{}
};

struct HalfedgeRendererView : public RendererView {
	HalfedgeRendererView() : 
		RendererView(Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))) 
	{}
};

struct VolumeRendererView : public RendererView {
	VolumeRendererView() : 
		RendererView(Shader(sl::shadersPath("volume.vert"), sl::shadersPath("volume.frag"))) 
	{}
};

struct SurfaceRendererView : public RendererView {
	SurfaceRendererView() : 
		RendererView(Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))) 
	{}
};

struct LineRendererView : public RendererView {
	LineRendererView() : 
		RendererView(Shader(sl::shadersPath("gizmo_line.vert"), sl::shadersPath("gizmo_line.frag"))) 
	{}
};

struct BBoxRendererView : public RendererView {
	BBoxRendererView() : 
		RendererView(Shader(sl::shadersPath("bbox.vert"), sl::shadersPath("bbox.frag"))) 
	{}
};

struct PolyRendererView : public RendererView {
	PolyRendererView() : 
		RendererView(Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))) 
	{}
};


struct PointSetRendererView : public RendererView {

	PointSetRendererView() : 
		RendererView(Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))) 
	{}

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