#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "surface_renderer.h"
#include "../element.h"
#include "../shader.h"

using namespace UM;

struct TriRenderer : public SurfaceRenderer {

	using SurfaceRenderer::SurfaceRenderer;
	void push() override;

};