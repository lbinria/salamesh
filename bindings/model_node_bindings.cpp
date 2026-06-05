#include "model_node_bindings.h"

namespace bindings {

	void ModelNodeBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<ModelNode>(
			"ModelNode",
			sol::base_classes, 
			sol::bases<SceneNode>(),
			"selected_colormap", sol::property(
				[](ModelNode &self) {
					return self.getSelectedColormap() + 1;
				}, 
				[](ModelNode &self, int idx) {
					self.setSelectedColormap(idx - 1);
				}
			)
		);
	}

}