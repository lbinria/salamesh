#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include "mesh_renderer.h"
#include "../element.h"
using namespace UM;

struct SurfaceRenderer : public MeshRenderer {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		int facetIndex;
	};

	SurfaceRenderer(std::string name, Surface &m) : 
		MeshRenderer(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}

	void init() override;
	void render(RendererView &rv, glm::vec3 &position) override;

	virtual void push() override = 0;
	void clear() override;
	void clean() override;

	MaterialInstance getDefaultMaterial() override;

	size_t getElementSize() override {
		return sizeof(Vertex);
	}

	std::vector<RendererElementField> getElementFields() override {
		return {
			{
				.name = "p0",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
				.offset = static_cast<int>(offsetof(Vertex, p0))
			},
			{
				.name = "p1",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
				.offset = static_cast<int>(offsetof(Vertex, p1))
			},
			{
				.name = "p2",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
				.offset = static_cast<int>(offsetof(Vertex, p2))
			},
			{
				.name = "facetIndex",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
				.offset = static_cast<int>(offsetof(Vertex, facetIndex))
			},
			{
				.name = "localIndex",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
				.offset = static_cast<int>(offsetof(Vertex, localIndex))
			},
			{
				.name = "cornerIndex",
				.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
				.offset = static_cast<int>(offsetof(Vertex, cornerIndex))
			}
		};
	}

	std::unique_ptr<RendererView> getDefaultView() override {
		auto rv = std::make_unique<SurfaceRendererView>();
		rv->visible = true;
		rv->setColor({0.71f, 0.71f, 0.71f});
		rv->setMeshShrink(0.f);
		rv->setMeshSize(0.0f);
		return rv;
	}


	protected:

	Surface &_m;

	private:

	void doLoadState(json &j) override {
		MeshRenderer::doLoadState(j);
	}
	void doSaveState(json &j) const override {
		MeshRenderer::doSaveState(j);
	}
};