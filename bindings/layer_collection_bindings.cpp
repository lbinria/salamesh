#include "layer_collection_bindings.h"

namespace bindings {

	void LayerCollectionBindings::loadBindings(sol::state &lua, IApp &app) {

		lua.new_usertype<LayerUnit>("LayerUnit",
			sol::bases<MaterialParams>(),
			sol::constructors<LayerUnit(Layer, ElementKind)>(),
			"get", &LayerUnit::get,
			"set", &LayerUnit::set,
			sol::meta_function::index, &LayerUnit::get,
			sol::meta_function::new_index, &LayerUnit::set
		);

		lua.new_usertype<LayerSet>("LayerSet",
			"unset", &LayerSet::unset,
			sol::meta_function::index, [](LayerSet &self, ElementKind kind) -> LayerUnit& {
				return self[kind];
			}
		);

		type = lua.new_usertype<LayerCollection>("LayerCollection",
			"unset", &LayerCollection::unset,
			sol::meta_function::index, [](LayerCollection &self, Layer layer) -> LayerSet& {
				return self[layer];
			}
		);


		
	}
}