#include "bbox_renderer.h"
#include "../../../core/graphic_api.h"
#include "../../helpers.h"

MaterialInstance BBoxRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam<StyleParams>("style");
	return mat;
}

std::vector<Renderer::RendererElementField> BBoxRenderer::getElementFields() {
	return {
		{
			.name = "p",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p))
		}
	};
}

Renderer::GeometricData BBoxRenderer::getData() {
	vertices.clear();
	vertices.reserve(32);

	// Search bbox
	glm::vec3 pMin{FLT_MAX, FLT_MAX, FLT_MAX};
	glm::vec3 pMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for (auto &p : ps) {
		glm::vec3 glmP = sl::um2glm(p);
		pMin = glm::min(pMin, glmP);
		pMax = glm::max(pMax, glmP);
	}

	// Create bbox lines
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

	nelements = vertices.size();
	return Renderer::GeometricData{ .vboBuffer = vertices.data(), .tboBuffers = {} };
}

void BBoxRenderer::clear() {
	vertices.clear();
	nelements = 0;
	requestUpdate();
}