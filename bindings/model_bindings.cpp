#include "model_bindings.h"

#include "../core/app_interface.h"
#include "../core/renderers/renderer.h"
#include "../core/renderers/point_set_renderer.h"
#include "../core/renderers/halfedge_renderer.h"
#include "../core/models/model.h"
#include "../core/models/surface_model.h"
#include "../core/attribute.h"

namespace bindings {

	void ModelBindings::loadBindings(sol::state &lua, IApp &app) {

		lua.new_enum("ElementKind", 
			"POINTS_ELT", ElementKind::POINTS_ELT,
			"CORNERS_ELT", ElementKind::CORNERS_ELT,
			"EDGES_ELT", ElementKind::EDGES_ELT,
			"FACETS_ELT", ElementKind::FACETS_ELT,
			"CELLS_ELT", ElementKind::CELLS_ELT,
			"CELL_FACETS_ELT", ElementKind::CELL_FACETS_ELT,
			"CELL_CORNERS_ELT", ElementKind::CELL_CORNERS_ELT
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

		// // Create container user types for different attribute types
		// sol::usertype<Attribute::Container<double>> attr_double_container_t = lua.new_usertype<Attribute::Container<double>>("AttributeContainerDouble");

		// attr_double_container_t[sol::meta_function::index] = [](Attribute::Container<double>& c, int i) {
		// 	return c[i];
		// };

		// attr_double_container_t[sol::meta_function::new_index] = [](Attribute::Container<double>& c, int i, double value) {
		// 	c[i] = value;
		// };

		// sol::usertype<Attribute::Container<int>> attr_int_container_t = lua.new_usertype<Attribute::Container<int>>("AttributeContainerInt");

		// attr_int_container_t[sol::meta_function::index] = [](Attribute::Container<int>& c, int i) {
		// 	return c[i];
		// };

		// attr_int_container_t[sol::meta_function::new_index] = [](Attribute::Container<int>& c, int i, int value) {
		// 	c[i] = value;
		// };


		sol::usertype<Attribute> attr_t = lua.new_usertype<Attribute>("Attribute", 
			sol::constructors<Attribute()>(),
			"name", sol::readonly_property(&Attribute::getName),
			"kind", sol::readonly_property(&Attribute::getKind),
			"type", sol::readonly_property(&Attribute::getType),
			"dim", sol::readonly_property(&Attribute::getDims),
			// "ptr", sol::readonly_property(&Attribute::getPtr),
			"double_container", sol::readonly_property(&Attribute::getContainer<double>),
			"int_container", sol::readonly_property(&Attribute::getContainer<int>),
			"bool_container", sol::readonly_property(&Attribute::getContainer<bool>),
			"vec2_container", sol::readonly_property(&Attribute::getContainer<vec2>),
			"vec3_container", sol::readonly_property(&Attribute::getContainer<vec3>)
		);


		sol::usertype<PointSetRenderer> pointSetRenderer_t = lua.new_usertype<PointSetRenderer>("PointSetRenderer");

		pointSetRenderer_t["visible"] = sol::property(
			&PointSetRenderer::getVisible,
			&PointSetRenderer::setVisible
		);
		pointSetRenderer_t["color"] = sol::property(
			&PointSetRenderer::getColor,
			&PointSetRenderer::setColor
		);
		pointSetRenderer_t["size"] = sol::property(
			&PointSetRenderer::getPointSize,
			&PointSetRenderer::setPointSize
		);

		sol::usertype<HalfedgeRenderer> halfedgeRenderer_t = lua.new_usertype<HalfedgeRenderer>("HalfedgeRenderer");

		halfedgeRenderer_t["visible"] = sol::property(
			&HalfedgeRenderer::getVisible,
			&HalfedgeRenderer::setVisible
		);
		halfedgeRenderer_t["inside_color"] = sol::property(
			&HalfedgeRenderer::getInsideColor,
			&HalfedgeRenderer::setInsideColor
		);
		halfedgeRenderer_t["outside_color"] = sol::property(
			&HalfedgeRenderer::getOutsideColor,
			&HalfedgeRenderer::setOutsideColor
		);
		halfedgeRenderer_t["thickness"] = sol::property(
			&HalfedgeRenderer::getThickness,
			&HalfedgeRenderer::setThickness
		);

		sol::usertype<IRenderer> meshRenderer_t = lua.new_usertype<IRenderer>("MeshRenderer");

		meshRenderer_t["visible"] = sol::property(
			&IRenderer::getVisible,
			&IRenderer::setVisible
		);

		meshRenderer_t["color"] = sol::property(
			&IRenderer::getColor,
			&IRenderer::setColor
		);

		meshRenderer_t["size"] = sol::property(
			&IRenderer::getMeshSize,
			&IRenderer::setMeshSize
		);

		meshRenderer_t["shrink"] = sol::property(
			&IRenderer::getMeshShrink,
			&IRenderer::setMeshShrink
		);

		meshRenderer_t["corner_visible"] = sol::property(
			&IRenderer::getCornerVisible,
			&IRenderer::setCornerVisible
		);

		meshRenderer_t["attr_repeat"] = sol::property(
			&IRenderer::getAttrRepeat,
			&IRenderer::setAttrRepeat
		);

		sol::usertype<Model> model_t = lua.new_usertype<Model>("Model");

		model_t.set_function("as_tri", &Model::as<TriModel>);
		model_t.set_function("as_quad", &Model::as<QuadModel>);
		model_t.set_function("as_poly", &Model::as<PolyModel>);

		// model_t["surface_attributes"] = sol::readonly_property([](Model &self) -> std::optional<SurfaceAttributes*> {
		// 	if (self.getModelType() == ModelType::TRI_MODEL) {
		// 		auto &m = self.as<TriModel>();
		// 		return &m.getSurfaceAttributes();
		// 	} else if (self.getModelType() == ModelType::QUAD_MODEL) {
		// 		auto &m = self.as<QuadModel>();
		// 		return &m.getSurfaceAttributes();
		// 	} else if (self.getModelType() == ModelType::POLYGON_MODEL) {
		// 		auto &m = self.as<PolyModel>();
		// 		return &m.getSurfaceAttributes();
		// 	}

		// 	return std::nullopt;
		// });

		// model_t["mesh"] = sol::readonly_property([](Model &self, sol::this_state s) -> sol::object {
		// 	sol::state_view lua(s);

		// 	if (self.getModelType() == ModelType::TRI_MODEL) {
		// 		auto &m = self.as<TriModel>();
		// 		auto &mesh = m.getMesh();
		// 		return sol::make_object(lua, mesh);
		// 	} else if (self.getModelType() == ModelType::QUAD_MODEL) {
		// 		auto &m = self.as<QuadModel>();
		// 		auto &mesh = m.getMesh();
		// 		return sol::make_object(lua, mesh);
		// 	} else if (self.getModelType() == ModelType::POLYGON_MODEL) {
		// 		auto &m = self.as<PolyModel>();
		// 		auto &mesh = m.getMesh();
		// 		return sol::make_object(lua, mesh);
		// 	}

		// 	return sol::nil;
		// });

		model_t.set_function("load", &Model::load);
		model_t.set_function("save", &Model::save);
		model_t.set_function("save_as", &Model::saveAs);

		model_t["name"] = sol::property(
			&Model::getName,
			&Model::setName
		);

		model_t["parent"] = sol::property(
			&Model::getParent,
			&Model::setParent
		);

		model_t["visible"] = sol::property(
			&Model::getVisible,
			&Model::setVisible
		);

		model_t["position"] = sol::property(
			&Model::getPosition,
			&Model::setPosition
		);

		model_t["world_position"] = sol::readonly_property(
			&Model::getWorldPosition
		);

		model_t["bbox"] = sol::readonly_property([](Model& m, sol::this_state s) {
			sol::state_view lua(s);
			auto [minv, maxv] = m.bbox();
			sol::table t = lua.create_table_with(1, minv, 2, maxv);
			return t;
		});

		model_t["center"] = sol::readonly_property(
			&Model::getCenter
		);

		model_t["radius"] = sol::readonly_property(
			&Model::getRadius
		);

		model_t["nverts"] = sol::readonly_property(
			&Model::nverts
		);

		model_t["nfacets"] = sol::readonly_property(
			&Model::nfacets
		);

		model_t["ncorners"] = sol::readonly_property(
			&Model::ncorners
		);

		model_t["nhalfedges"] = sol::readonly_property(
			&Model::nhalfedges
		);

		model_t["ncells"] = sol::readonly_property(
			&Model::ncells
		);


		model_t["light"] = sol::property(
			&Model::getLight,
			&Model::setLight
		);

		model_t["is_light_follow_view"] = sol::property(
			&Model::getLightFollowView,
			&Model::setLightFollowView
		);

		model_t["clipping_mode"] = sol::property([](Model &self) {
			return self.getClippingMode() + 1;
		}, [](Model &self, int selected) {
			self.setClippingMode(static_cast<IRenderer::ClippingMode>(selected - 1));
		});

		model_t["clipping"] = sol::property(
			&Model::getClipping,
			&Model::setClipping
		);

		model_t["clipping_plane_point"] = sol::property(
			&Model::getClippingPlanePoint,
			&Model::setClippingPlanePoint
		);

		model_t["clipping_plane_normal"] = sol::property(
			&Model::getClippingPlaneNormal,
			&Model::setClippingPlaneNormal
		);

		model_t["invert_clipping"] = sol::property(
			&Model::getInvertClipping,
			&Model::setInvertClipping
		);

		model_t.set_function("setup_clipping", &Model::setupClipping);

		model_t["clipping_mode_strings"] = sol::readonly_property(&Model::getClippingModeStrings);

		// Renderers accesses
		model_t["points"] = sol::readonly_property(&Model::getPointsRenderer);
		model_t["edges"] = sol::readonly_property(&Model::getEdgesRenderer);
		model_t["mesh"] = sol::readonly_property(&Model::getMeshRenderer);

		model_t["attrs"] = sol::readonly_property([&lua = lua](Model &self) {
			sol::table attrs_tbl = lua.create_table();
			for (auto &attr : self.getAttrs()) {
				sol::table item = lua.create_table();
				attrs_tbl.add(attr);
			}
			return attrs_tbl;
		});

		model_t["selected_attr0"] = sol::property([](Model &self) {
			return self.getSelectedAttr(ColormapLayer::COLORMAP_LAYER_0) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedAttr(selected - 1, ColormapLayer::COLORMAP_LAYER_0);
		});

		model_t["selected_attr1"] = sol::property([](Model &self) {
			return self.getSelectedAttr(ColormapLayer::COLORMAP_LAYER_1) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedAttr(selected - 1, ColormapLayer::COLORMAP_LAYER_1);
		});

		model_t["selected_attr2"] = sol::property([](Model &self) {
			return self.getSelectedAttr(ColormapLayer::COLORMAP_LAYER_2) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedAttr(selected - 1, ColormapLayer::COLORMAP_LAYER_2);
		});

		

		

		model_t["selected_colormap0"] = sol::property([](Model &self) {
			return self.getSelectedColormap(ColormapLayer::COLORMAP_LAYER_0) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedColormap(selected - 1, ColormapLayer::COLORMAP_LAYER_0);
		});

		model_t["selected_colormap1"] = sol::property([](Model &self) {
			return self.getSelectedColormap(ColormapLayer::COLORMAP_LAYER_1) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedColormap(selected - 1, ColormapLayer::COLORMAP_LAYER_1);
		});

		model_t["selected_colormap2"] = sol::property([](Model &self) {
			return self.getSelectedColormap(ColormapLayer::COLORMAP_LAYER_2) + 1;
		}, [](Model &self, int selected) {
			self.setSelectedColormap(selected - 1, ColormapLayer::COLORMAP_LAYER_2);
		});

		// Layers
		// TODO miss some functions around layers !
		model_t.set_function("set_colormap_attr", &Model::setColormapAttr);
		model_t.set_function("set_colormap", &Model::setColormap);
		model_t.set_function("unset_colormap", &Model::unsetColormap);
		// TODO test
		model_t.set_function("unset_colormaps", sol::overload(
			[](Model &self) { self.unsetColormaps(); },
			[](Model &self, ElementKind kind) { self.unsetColormaps(kind); },
			[](Model &self, ColormapLayer layer) { self.unsetColormaps(layer); }
		));


		model_t.set_function("set_highlight_attr", &Model::setHighlightAttr);
		model_t.set_function("set_highlight", &Model::setHighlight);
		model_t.set_function("unset_highlight", &Model::unsetHighlight);
		model_t.set_function("unset_highlights", &Model::unsetHighlights);

		model_t.set_function("set_filter_attr", &Model::setFilterAttr);
		model_t.set_function("set_filter", &Model::setFilter);
		model_t.set_function("unset_filter", &Model::unsetFilter);
		model_t.set_function("unset_filters", &Model::unsetFilters);


		// TODO do the same for other types of meshes
		// & maybe refactor, we can add these properties directly to Model I think
		auto tri_model_t = lua.new_usertype<TriModel>("TriModel", sol::base_classes, sol::bases<Model>());
		tri_model_t["surface_attributes"] = sol::readonly_property([](TriModel &self) -> SurfaceAttributes& { return self.getSurfaceAttributes(); });
		tri_model_t["mesh"] = sol::readonly_property([](TriModel &self) -> Surface& { return self.getMesh(); });

		auto quad_model_t = lua.new_usertype<QuadModel>("QuadModel", sol::base_classes, sol::bases<Model>());
		quad_model_t["surface_attributes"] = sol::readonly_property([](QuadModel &self) -> SurfaceAttributes& { return self.getSurfaceAttributes(); });
		quad_model_t["mesh"] = sol::readonly_property([](QuadModel &self) -> Surface& { return self.getMesh(); });

		auto poly_model_t = lua.new_usertype<PolyModel>("PolyModel", sol::base_classes, sol::bases<Model>());
		poly_model_t["surface_attributes"] = sol::readonly_property([](PolyModel &self) -> SurfaceAttributes& { return self.getSurfaceAttributes(); });
		poly_model_t["mesh"] = sol::readonly_property([](PolyModel &self) -> Surface& { return self.getMesh(); });
	}

}