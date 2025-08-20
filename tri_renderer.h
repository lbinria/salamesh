#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/model.h"
#include "surface_renderer.h"
#include "core/element.h"
#include "shader.h"
#include "vertex.h"

// TODO IMPORTANT see to bind mesh directly to the shader via buffermap and pointers

using namespace UM;

struct TriRenderer : public SurfaceRenderer {

	using SurfaceRenderer::SurfaceRenderer;
	void push() override;

};