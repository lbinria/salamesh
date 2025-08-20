#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "volume_renderer.h"
#include "core/model.h"
#include "core/element.h"
#include "shader.h"
#include "vertex.h"

using namespace UM;

struct HexRenderer : public VolumeRenderer {
	using VolumeRenderer::VolumeRenderer;
	void push() override;
};