#include "geometry_bindings.h"

namespace bindings {

	void GeometryBindings::loadBindings(sol::state &lua, IApp &app) {

		auto castToMeshGeo = [&lua](Geometry &self, auto getter) -> sol::object {
			auto meshGeo = dynamic_cast<MeshGeometry*>(&self);
			if (meshGeo) {
				return sol::make_object(lua, getter(meshGeo));
			}
			return sol::nil;
		};

		type = lua.new_usertype<Geometry>("Geometry",
			"bbox", sol::readonly_property(&Geometry::bbox),
			"center", sol::readonly_property(&Geometry::getCenter),
			"radius", sol::readonly_property(&Geometry::getRadius),
			"save", &Geometry::save,
			"saveAs", &Geometry::saveAs,
			"request_update", &Geometry::requestUpdate,
			"nverts", sol::property([&](Geometry &self) {
				return castToMeshGeo(self, [](MeshGeometry* m) { return m->nverts(); });
			}),
			"ncorners", sol::property([&](Geometry &self) {
				return castToMeshGeo(self, [](MeshGeometry* m) { return m->ncorners(); });
			}),
			"nhalfedges", sol::property([&](Geometry &self) {
				return castToMeshGeo(self, [](MeshGeometry* m) { return m->nhalfedges(); });
			}),
			"nfacets", sol::property([&](Geometry &self) {
				return castToMeshGeo(self, [](MeshGeometry* m) { return m->nfacets(); });
			}),
			"ncells", sol::property([&](Geometry &self) {
				return castToMeshGeo(self, [](MeshGeometry* m) { return m->ncells(); });
			})
		);
	}
}

