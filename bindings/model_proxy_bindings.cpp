#include "model_proxy_bindings.h"

namespace bindings {

	void ModelProxyBindings::loadBindings(sol::state &lua, IApp &app) {
		// type = lua.new_usertype<ModelProxy>("ModelProxy",
		// 	"selected_colormap", sol::property(
		// 		[](ModelProxy &self) {
		// 			return self.getSelectedColormap() + 1;
		// 		}, 
		// 		[](ModelProxy &self, int idx) {
		// 			self.setSelectedColormap(idx - 1);
		// 		}
		// 	)
		// );
	}
}