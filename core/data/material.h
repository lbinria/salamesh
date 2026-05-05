#pragma once

#include "../shader.h"

struct MaterialParams {

	virtual void apply(Shader &shader) const = 0;

};