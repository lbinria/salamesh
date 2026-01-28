#include "um_bindings.h"

#include <filesystem>
#include "../helpers/graphic_api.h"
#include "../core/helpers.h"
#include <ultimaille/all.h>

using namespace UM;
namespace fs = std::filesystem;

namespace bindings {

	// Note that we can refactor by merging the three following functions
	// But we don't understand anything after that
	template<typename T, template<typename> class TAttr>
	static void bindAttributeFunctionsSurface(sol::usertype<TAttr<T>> &user_type) {
		
		user_type.set_function("bind", [](TAttr<T> &self, std::string name, SurfaceAttributes &attr, Surface &m) {
			self.bind(name, attr, m);
		});

		user_type.set_function("fill", [](TAttr<T> &self, T val) {
			self.fill(val);
		});

		user_type[sol::meta_function::index] = [](TAttr<T>& self, int i) -> T {
			return self[i];
		};

		user_type[sol::meta_function::new_index] = [](TAttr<T>& self, int i, T value) {
			self[i] = value;
		};
	}

	template<typename T, template<typename> class TAttr>
	static void bindAttributeFunctionsVolume(sol::usertype<TAttr<T>> &user_type) {
		
		user_type.set_function("bind", [](TAttr<T> &self, std::string name, VolumeAttributes &attr, Volume &m) {
			self.bind(name, attr, m);
		});

		user_type.set_function("fill", [](TAttr<T> &self, T val) {
			self.fill(val);
		});

		user_type[sol::meta_function::index] = [](TAttr<T>& self, int i) -> T {
			return self[i];
		};

		user_type[sol::meta_function::new_index] = [](TAttr<T>& self, int i, T value) {
			self[i] = value;
		};
	}

	template<typename T, template<typename> class TAttr>
	static void bindAttributeFunctionsPolyline(sol::usertype<TAttr<T>> &user_type) {
		
		user_type.set_function("bind", [](TAttr<T> &self, std::string name, PolyLineAttributes &attr, PolyLine &m) {
			self.bind(name, attr, m);
		});

		user_type.set_function("fill", [](TAttr<T> &self, T val) {
			self.fill(val);
		});

		user_type[sol::meta_function::index] = [](TAttr<T>& self, int i) -> T {
			return self[i];
		};

		user_type[sol::meta_function::new_index] = [](TAttr<T>& self, int i, T value) {
			self[i] = value;
		};
	}

	// Create a lua usertype with sol for attribute: example a PointAttribute<double> as "PointAttributeDouble"
	template<typename T, template<typename> class TAttr>
	static sol::usertype<TAttr<T>> bindAttributeType(sol::state &lua, std::string strTypename) {
		sol::usertype<TAttr<T>> user_type = lua.new_usertype<TAttr<T>>(strTypename,
			sol::constructors<TAttr<T>(), TAttr<T>(T def)>()
		);
		return user_type;
	}

	void UMBindings::loadBindings(sol::state &lua, IApp &app) {


		sol::usertype<Surface> surface_t = lua.new_usertype<Surface>("Surface");
		sol::usertype<Volume> volume_t = lua.new_usertype<Volume>("Volume");

		// Bind edge attribute
		auto edge_attr_bool_t = bindAttributeType<bool, EdgeAttribute>(lua, "EdgeAttributeBool");
		auto edge_attr_int_t = bindAttributeType<int, EdgeAttribute>(lua, "EdgeAttributeInt");
		auto edge_attr_double_t = bindAttributeType<double, EdgeAttribute>(lua, "EdgeAttributeDouble");
		auto edge_attr_vec2_t = bindAttributeType<vec2, EdgeAttribute>(lua, "EdgeAttributeVec2");
		auto edge_attr_vec3_t = bindAttributeType<vec3, EdgeAttribute>(lua, "EdgeAttributeVec3");

		bindAttributeFunctionsPolyline(edge_attr_bool_t);
		bindAttributeFunctionsPolyline(edge_attr_int_t);
		bindAttributeFunctionsPolyline(edge_attr_double_t);
		bindAttributeFunctionsPolyline(edge_attr_vec2_t);
		bindAttributeFunctionsPolyline(edge_attr_vec3_t);

		// Bind point attribute
		auto point_attr_bool_t = bindAttributeType<bool, PointAttribute>(lua, "PointAttributeBool");
		auto point_attr_int_t = bindAttributeType<int, PointAttribute>(lua, "PointAttributeInt");
		auto point_attr_double_t = bindAttributeType<double, PointAttribute>(lua, "PointAttributeDouble");
		auto point_attr_vec2_t = bindAttributeType<vec2, PointAttribute>(lua, "PointAttributeVec2");
		auto point_attr_vec3_t = bindAttributeType<vec3, PointAttribute>(lua, "PointAttributeVec3");

		bindAttributeFunctionsSurface(point_attr_bool_t);
		bindAttributeFunctionsSurface(point_attr_int_t);
		bindAttributeFunctionsSurface(point_attr_double_t);
		bindAttributeFunctionsSurface(point_attr_vec2_t);
		bindAttributeFunctionsSurface(point_attr_vec3_t);

		// Bind corner attribute
		auto corner_attr_bool_t = bindAttributeType<bool, CornerAttribute>(lua, "CornerAttributeBool");
		auto corner_attr_int_t = bindAttributeType<int, CornerAttribute>(lua, "CornerAttributeInt");
		auto corner_attr_double_t = bindAttributeType<double, CornerAttribute>(lua, "CornerAttributeDouble");
		auto corner_attr_vec2_t = bindAttributeType<vec2, CornerAttribute>(lua, "CornerAttributeVec2");
		auto corner_attr_vec3_t = bindAttributeType<vec3, CornerAttribute>(lua, "CornerAttributeVec3");

		bindAttributeFunctionsSurface(corner_attr_bool_t);
		bindAttributeFunctionsSurface(corner_attr_int_t);
		bindAttributeFunctionsSurface(corner_attr_double_t);
		bindAttributeFunctionsSurface(corner_attr_vec2_t);
		bindAttributeFunctionsSurface(corner_attr_vec3_t);

		// Bind facet attribute
		auto facet_attr_bool_t = bindAttributeType<bool, FacetAttribute>(lua, "FacetAttributeBool");
		auto facet_attr_int_t = bindAttributeType<int, FacetAttribute>(lua, "FacetAttributeInt");
		auto facet_attr_double_t = bindAttributeType<double, FacetAttribute>(lua, "FacetAttributeDouble");
		auto facet_attr_vec2_t = bindAttributeType<vec2, FacetAttribute>(lua, "FacetAttributeVec2");
		auto facet_attr_vec3_t = bindAttributeType<vec3, FacetAttribute>(lua, "FacetAttributeVec3");

		bindAttributeFunctionsSurface(facet_attr_bool_t);
		bindAttributeFunctionsSurface(facet_attr_int_t);
		bindAttributeFunctionsSurface(facet_attr_double_t);
		bindAttributeFunctionsSurface(facet_attr_vec2_t);
		bindAttributeFunctionsSurface(facet_attr_vec3_t);

		// Bind Cell corner attribute
		auto cell_corner_attr_bool = bindAttributeType<bool, CellCornerAttribute>(lua, "CellCornerAttributeBool");
		auto cell_corner_attr_int = bindAttributeType<int, CellCornerAttribute>(lua, "CellCornerAttributeInt");
		auto cell_corner_attr_double = bindAttributeType<double, CellCornerAttribute>(lua, "CellCornerAttributeDouble");
		auto cell_corner_attr_vec2 = bindAttributeType<vec2, CellCornerAttribute>(lua, "CellCornerAttributeVec2");
		auto cell_corner_attr_vec3 = bindAttributeType<vec3, CellCornerAttribute>(lua, "CellCornerAttributeVec3");

		bindAttributeFunctionsVolume(cell_corner_attr_bool);
		bindAttributeFunctionsVolume(cell_corner_attr_int);
		bindAttributeFunctionsVolume(cell_corner_attr_double);
		bindAttributeFunctionsVolume(cell_corner_attr_vec2);
		bindAttributeFunctionsVolume(cell_corner_attr_vec3);

		// Bind Cell facet attribute
		auto cell_facet_attr_bool = bindAttributeType<bool, CellFacetAttribute>(lua, "CellFacetAttributeBool");
		auto cell_facet_attr_int = bindAttributeType<int, CellFacetAttribute>(lua, "CellFacetAttributeInt");
		auto cell_facet_attr_double = bindAttributeType<double, CellFacetAttribute>(lua, "CellFacetAttributeDouble");
		auto cell_facet_attr_vec2 = bindAttributeType<vec2, CellFacetAttribute>(lua, "CellFacetAttributeVec2");
		auto cell_facet_attr_vec3 = bindAttributeType<vec3, CellFacetAttribute>(lua, "CellFacetAttributeVec3");

		bindAttributeFunctionsVolume(cell_facet_attr_bool);
		bindAttributeFunctionsVolume(cell_facet_attr_int);
		bindAttributeFunctionsVolume(cell_facet_attr_double);
		bindAttributeFunctionsVolume(cell_facet_attr_vec2);
		bindAttributeFunctionsVolume(cell_facet_attr_vec3);

		// Bind Cell attribute
		auto cell_attr_bool = bindAttributeType<bool, CellAttribute>(lua, "CellAttributeBool");
		auto cell_attr_int = bindAttributeType<int, CellAttribute>(lua, "CellAttributeInt");
		auto cell_attr_double = bindAttributeType<double, CellAttribute>(lua, "CellAttributeDouble");
		auto cell_attr_vec2 = bindAttributeType<vec2, CellAttribute>(lua, "CellAttributeVec2");
		auto cell_attr_vec3 = bindAttributeType<vec2, CellAttribute>(lua, "CellAttributeVec3");

		bindAttributeFunctionsVolume(cell_attr_bool);
		bindAttributeFunctionsVolume(cell_attr_int);
		bindAttributeFunctionsVolume(cell_attr_double);
		bindAttributeFunctionsVolume(cell_attr_vec2);
		bindAttributeFunctionsVolume(cell_attr_vec3);

	}
}