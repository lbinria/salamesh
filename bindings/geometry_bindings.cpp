#include "geometry_bindings.h"

namespace bindings {

	void GeometryBindings::loadBindings(sol::state &lua, IApp &app) {
		
		type = lua.new_usertype<Geometry>("Geometry",
			"bbox", sol::readonly_property(&Geometry::bbox),
			"center", sol::readonly_property(&Geometry::getCenter),
			"radius", sol::readonly_property(&Geometry::getRadius),
			"save", &Geometry::save,
			"saveAs", &Geometry::saveAs,
			"request_update", &Geometry::requestUpdate,
			"nverts",  sol::property([&lua](Geometry &self) -> sol::object {
				auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nverts());
				}
				return sol::nil;
			}),
			"ncorners",  sol::property([&lua](Geometry &self) -> sol::object {
				auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->ncorners());
				}
				return sol::nil;
			}),
			"nhalfedges",  sol::property([&lua](Geometry &self) -> sol::object {
				auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nhalfedges());
				}
				return sol::nil;
			}),
			"nfacets",  sol::property([&lua](Geometry &self) -> sol::object {
				auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->nfacets());
				}
				return sol::nil;
			}),
			"ncells",  sol::property([&lua](Geometry &self) -> sol::object {
				auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
				if (meshGeo) {
					return sol::make_object(lua, meshGeo->ncells());
				}
				return sol::nil;
			})
		);

		// auto castToMeshGeo = [&lua](Geometry &self, auto getter) -> sol::object {
		// 	auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
		// 	if (meshGeo) {
		// 		return sol::make_object(lua, getter(meshGeo));
		// 	}
		// 	return sol::nil;
		// };

		// type = lua.new_usertype<Geometry>("Geometry",
		// 	"bbox", sol::readonly_property(&Geometry::bbox),
		// 	"center", sol::readonly_property(&Geometry::getCenter),
		// 	"radius", sol::readonly_property(&Geometry::getRadius),
		// 	"save", &Geometry::save,
		// 	"saveAs", &Geometry::saveAs,
		// 	"request_update", &Geometry::requestUpdate,
		// 	"nverts", sol::property([&](Geometry &self) {
		// 		return castToMeshGeo(self, [](MeshGeometry* m) { return m->nverts(); });
		// 	}),
		// 	"ncorners", sol::property([&](Geometry &self) {
		// 		return castToMeshGeo(self, [](MeshGeometry* m) { return m->ncorners(); });
		// 	}),
		// 	"nhalfedges", sol::property([&](Geometry &self) {
		// 		return castToMeshGeo(self, [](MeshGeometry* m) { return m->nhalfedges(); });
		// 	}),
		// 	"nfacets", sol::property([&](Geometry &self) {
		// 		return castToMeshGeo(self, [](MeshGeometry* m) { return m->nfacets(); });
		// 	}),
		// 	"ncells", sol::property([&](Geometry &self) {
		// 		return castToMeshGeo(self, [](MeshGeometry* m) { return m->ncells(); });
		// 	})
		// );
	}
}

