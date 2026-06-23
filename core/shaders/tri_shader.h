#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "surface_shader.h"
#include "element_type.h"
#include "../shaders/shader.h"

using namespace UM;

struct TriShader : public SurfaceShader {

	using SurfaceShader::SurfaceShader;

};