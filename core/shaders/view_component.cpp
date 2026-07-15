#include "view_component.h"
#include "shader_base.h"

void ViewComponent::update(Mesh &mesh) {
	_shader.update(_meshBuffer, mesh);
}