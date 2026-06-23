#include "model_bindings.h"

#include "../core/app_interface.h"
#include "attribute.h"

namespace bindings {

	template<typename T>
	void createAttributeContainerType(sol::state &lua, std::string name) {
		lua.new_usertype<Attribute::Container<T>>(name, 
			sol::meta_function::index, [&lua](Attribute::Container<T>& self, int i) {
				return self.get(i);
			},
			sol::meta_function::new_index, [&lua](Attribute::Container<T>& self, int i, T val) {
				self.set(i, val);
			}
		);
	}

	void ModelBindings::loadBindings(sol::state &lua, IApp &app) {

		lua.new_enum("ModelType", 
			"POINTSET_MODEL", ModelType::POINTSET_MODEL,
			"POLYLINE_MODEL", ModelType::POLYLINE_MODEL,
			"TRI_MODEL", ModelType::TRI_MODEL,
			"QUAD_MODEL", ModelType::QUAD_MODEL,
			"POLYGON_MODEL", ModelType::POLYGON_MODEL,
			"TET_MODEL", ModelType::TET_MODEL,
			"HEX_MODEL", ModelType::HEX_MODEL,
			"PYRAMID_MODEL", ModelType::PYRAMID_MODEL,
			"PRISM_MODEL", ModelType::PRISM_MODEL,
			"HYBRID_MODEL", ModelType::HYBRID_MODEL
		);

		lua.new_enum("ElementKind", 
			"POINTS_ELT", ElementKind::POINTS_ELT,
			"CORNERS_ELT", ElementKind::CORNERS_ELT,
			"EDGES_ELT", ElementKind::EDGES_ELT,
			"FACETS_ELT", ElementKind::FACETS_ELT,
			"CELLS_ELT", ElementKind::CELLS_ELT,
			"CELL_FACETS_ELT", ElementKind::CELL_FACETS_ELT,
			"CELL_CORNERS_ELT", ElementKind::CELL_CORNERS_ELT
		);

		lua.new_enum("ElementType", 
			"DOUBLE_ELT", ElementType::DOUBLE_ELT,
			"INT_ELT", ElementType::INT_ELT,
			"BOOL_ELT", ElementType::BOOL_ELT,
			"VEC2_ELT", ElementType::VEC2_ELT,
			"VEC3_ELT", ElementType::VEC3_ELT
		);

		lua.new_enum("Layer", 
			"COLORMAP_0", Layer::COLORMAP_0,
			"COLORMAP_1", Layer::COLORMAP_1,
			"COLORMAP_2", Layer::COLORMAP_2,
			"HIGHLIGHT", Layer::HIGHLIGHT,
			"FILTER", Layer::FILTER
		);

		lua.new_enum("ColormapLayer", 
			"COLORMAP_LAYER_0", ColormapLayer::COLORMAP_LAYER_0,
			"COLORMAP_LAYER_1", ColormapLayer::COLORMAP_LAYER_1,
			"COLORMAP_LAYER_2", ColormapLayer::COLORMAP_LAYER_2
		);



		sol::usertype<Attribute> attr_t = lua.new_usertype<Attribute>("Attribute", 
			sol::constructors<Attribute()>(),
			"name", sol::readonly_property(&Attribute::getName),
			"kind", sol::readonly_property(&Attribute::getKind),
			"type", sol::readonly_property(&Attribute::getType),
			"dim", sol::readonly_property(&Attribute::getNDims),
			"double_data", sol::readonly_property(&Attribute::getContainer<double>),
			"int_data", sol::readonly_property(&Attribute::getContainer<int>),
			"bool_data", sol::readonly_property(&Attribute::getContainer<bool>),
			"vec2_data", sol::readonly_property(&Attribute::getContainer<vec2>),
			"vec3_data", sol::readonly_property(&Attribute::getContainer<vec3>)
		);

		createAttributeContainerType<double>(lua, "DoubleContainer");
		createAttributeContainerType<int>(lua, "IntContainer");
		createAttributeContainerType<bool>(lua, "BoolContainer");
		createAttributeContainerType<vec2>(lua, "Vec2Container");
		createAttributeContainerType<vec3>(lua, "Vec3Container");
	}

}