#include "mesh_bindings.h"

namespace bindings {

	void MeshBindings::loadBindings(sol::state &lua, IApp &app) {
		
		type = lua.new_usertype<Mesh>("Mesh",
			"bbox", sol::readonly_property([](Mesh& self, sol::this_state s) {
				sol::state_view lua(s);
				auto [minv, maxv] = self.bbox();
				sol::table t = lua.create_table_with(1, minv, 2, maxv);
				return t;
			}),
			"center", sol::readonly_property(&Mesh::getCenter),
			"radius", sol::readonly_property(&Mesh::getRadius),
			"attributes", sol::readonly_property(&Mesh::getAttributes),
			"get_attribute", &Mesh::getAttribute,
			"save", &Mesh::save,
			"saveAs", &Mesh::saveAs,
			"nverts",  sol::property([&lua](Mesh &self) -> sol::object {
				auto meshGeo = dynamic_cast<Mesh*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nverts());
				}
				return sol::nil;
			}),
			"ncorners",  sol::property([&lua](Mesh &self) -> sol::object {
				auto meshGeo = dynamic_cast<Mesh*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->ncorners());
				}
				return sol::nil;
			}),
			"nhalfedges",  sol::property([&lua](Mesh &self) -> sol::object {
				auto meshGeo = dynamic_cast<Mesh*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nhalfedges());
				}
				return sol::nil;
			}),
			"nfacets",  sol::property([&lua](Mesh &self) -> sol::object {
				auto meshGeo = dynamic_cast<Mesh*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nfacets());
				}
				return sol::nil;
			}),
			"ncells",  sol::property([&lua](Mesh &self) -> sol::object {
				auto meshGeo = dynamic_cast<Mesh*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->ncells());
				}
				return sol::nil;
			})
		);

		// auto castToMeshGeo = [&lua](Mesh &self, auto getter) -> sol::object {
		// 	auto meshGeo = dynamic_cast<Mesh*>(&self);
		// 	if (meshGeo) {
		// 		return sol::make_object(lua, getter(meshGeo));
		// 	}
		// 	return sol::nil;
		// };

		// type = lua.new_usertype<Mesh>("Mesh",
		// 	"bbox", sol::readonly_property(&Mesh::bbox),
		// 	"center", sol::readonly_property(&Mesh::getCenter),
		// 	"radius", sol::readonly_property(&Mesh::getRadius),
		// 	"save", &Mesh::save,
		// 	"saveAs", &Mesh::saveAs,
		// 	"request_update", &Mesh::requestUpdate,
		// 	"nverts", sol::property([&](Mesh &self) {
		// 		return castToMeshGeo(self, [](Mesh* m) { return m->nverts(); });
		// 	}),
		// 	"ncorners", sol::property([&](Mesh &self) {
		// 		return castToMeshGeo(self, [](Mesh* m) { return m->ncorners(); });
		// 	}),
		// 	"nhalfedges", sol::property([&](Mesh &self) {
		// 		return castToMeshGeo(self, [](Mesh* m) { return m->nhalfedges(); });
		// 	}),
		// 	"nfacets", sol::property([&](Mesh &self) {
		// 		return castToMeshGeo(self, [](Mesh* m) { return m->nfacets(); });
		// 	}),
		// 	"ncells", sol::property([&](Mesh &self) {
		// 		return castToMeshGeo(self, [](Mesh* m) { return m->ncells(); });
		// 	})
		// );
	}
}

