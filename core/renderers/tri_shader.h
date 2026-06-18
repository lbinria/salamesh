#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "surface_shader.h"
#include "../data/element_type.h"
#include "../shader.h"

using namespace UM;

struct TriShader : public SurfaceShader {

	using SurfaceShader::SurfaceShader;
	void push() override;

};