#include "line_renderer.h"
#include "../helpers.h"
#include "../../core/graphic_api.h"

std::vector<Renderer::RendererElementField> LineRenderer::getElementFields() {
	return {
		{
			.name = "p",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(LineComponent, p))
		},
		{
			.name = "color",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(LineComponent, color))
		}
	};
}

Renderer::GeometricData LineRenderer::getData() {

	lineComponents.clear();

	// Map Line -> LineComponent for VBO
	for (auto &l : lines) {
		lineComponents.push_back({.p = l.a, .color = l.color});
		lineComponents.push_back({ .p = l.b, .color = l.color });
	}

	nelements = lineComponents.size();

	return Renderer::GeometricData{ .vboBuffer = lineComponents.data(), .texBuffers = {} };
}

void LineRenderer::clear() {
	clearLines();
}