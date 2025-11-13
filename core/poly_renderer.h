#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "model.h"
#include "surface_renderer_2.h"
#include "element.h"
#include "shader.h"

using namespace UM;

struct PolyRenderer : public SurfaceRenderer2 {

	using SurfaceRenderer2::SurfaceRenderer2;
	void push() override;

};