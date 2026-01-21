#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <filesystem>
#include "../helpers/graphic_api.h"
#include "../core/helpers.h"
#include <ultimaille/all.h>

using namespace UM;

namespace fs = std::filesystem;

namespace bindings {

	struct UMBindings {

		static void loadBindings(sol::state &lua) {

			sol::usertype<FacetAttribute<double>> facet_attr_double_t = lua.new_usertype<FacetAttribute<double>>("FacetAttributeDouble",
				sol::constructors<FacetAttribute<double>()>()
			);

			sol::usertype<Surface> surface_t = lua.new_usertype<Surface>("Surface");

			
			// CornerAttribute<double> ca;
			// ca.bind("corner_id", triModel.getSurfaceAttributes(), triModel.getMesh());

			// facet_attr_double_t.set_function("bind", &FacetAttribute<double>::bind);
			facet_attr_double_t.set_function("bind", [](FacetAttribute<double> &self, std::string name, SurfaceAttributes &attr, Surface &m) {
				self.bind(name, attr, m);
			});

			facet_attr_double_t[sol::meta_function::index] = [](FacetAttribute<double>& self, int i) {
				return self[i];
			};

			facet_attr_double_t[sol::meta_function::new_index] = [](FacetAttribute<double>& self, int i, double value) {
				self[i] = value;
			};

		}

	};

}