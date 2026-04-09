#pragma once

#include "renderer.h"

#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeRenderer : public Renderer {

	struct LineVert {
		int halfedgeIndex;
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
		glm::vec3 bary;
	};

	using Renderer::Renderer;

	void init() override;
	virtual void push() = 0;
	void render(RendererView &rv, glm::vec3 &position) override;
	void clear() override;
	void clean() override;

	std::unique_ptr<RendererView> getDefaultView() override { 
		auto rv = std::make_unique<HalfedgeRendererView>(); 
		rv->setThickness(2.0f);
		rv->setInsideColor({0.0, 0.97, 0.73});
		rv->setOutsideColor({0.0, 0.6, 0.45});
		return rv;
	}

	MaterialInstance getDefaultMaterial() override;
	std::vector<RendererElementField> getElementFields() override;

	size_t getElementSize() override {
		return sizeof(LineVert);
	}

	RenderPrimitive getRenderPrimitive() const override {
		return RenderPrimitive::RENDER_TRIANGLES;
	}


	protected:

	std::vector<LineVert> vertices;

	private:



	void doLoadState(json &j) override {

	}

	void doSaveState(json &j) const override {

	}
	
};

struct SurfaceHalfedgeRenderer : public HalfedgeRenderer {

	SurfaceHalfedgeRenderer(std::string name, Surface &m) : 
		HalfedgeRenderer(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {}

	void push() override;
	const void * getData() override;

	Surface &_m;



};

struct VolumeHalfedgeRenderer : public HalfedgeRenderer {

	VolumeHalfedgeRenderer(std::string name, Volume &m) : 
		HalfedgeRenderer(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {}

	void push() override;
	const void * getData() override;

	Volume &_m;
};

struct PolylineRenderer : public HalfedgeRenderer {

		PolylineRenderer(std::string name, PolyLine &m) : 
		HalfedgeRenderer(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {}

	void push() override;
	const void * getData() override;

	PolyLine &_m;
};