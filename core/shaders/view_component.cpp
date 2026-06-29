#include "view_component.h"
#include "shader_base.h"

void ViewComponent::update(Geometry &geometry) {
	_shader.update(_geometryBuffer, geometry);
}