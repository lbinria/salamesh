#include "app_bindings.h"

#include "../core/element.h"
#include "../core/render_system.h"

namespace bindings {

	void AppBindings::loadBindings(sol::state &lua, IApp &app) {

		// Image type binding
		sol::usertype<Image> image_t = lua.new_usertype<Image>("Image", 
			sol::constructors<Image()>(),
			"tex_id", sol::readonly_property(&Image::getTexId),
			"width", sol::readonly_property(&Image::getWidth),
			"height", sol::readonly_property(&Image::getHeight),
			"channels", sol::readonly_property(&Image::getChannels)
		);

		// Snapshot type binding
		sol::usertype<Snapshot> snapshot_t = lua.new_usertype<Snapshot>("Snapshot", 
			sol::constructors<Snapshot()>(),
			"state_filename", sol::readonly_property(&Snapshot::getStateFilename),
			"thumb_filename", sol::readonly_property(&Snapshot::getThumbfilename),
			"image", sol::readonly_property(&Snapshot::getImage)
		);

		// Instanciator type binding
		lua.new_usertype<Instanciator<Model>>("ModelInstanciator", 
			// "register_type", &Instanciator<Model>::registerType,
			"list_available_types", &Instanciator<Model>::listAvailableTypes
		);
		lua.new_usertype<Instanciator<Camera>>("CameraInstanciator", 
			// "register_type", &Instanciator<Camera>::registerType,
			"list_available_types", &Instanciator<Camera>::listAvailableTypes
		);
		lua.new_usertype<Instanciator<Renderer>>("RendererInstanciator", 
			// "register_type", &Instanciator<Renderer>::registerType,
			"list_available_types", &Instanciator<Renderer>::listAvailableTypes
		);

		// Input state binding
		sol::usertype<InputState::HoverState> primitive_state_t = lua.new_usertype<InputState::HoverState>("HoverState", 
			sol::constructors<InputState::HoverState()>(),
			"hovered", sol::readonly_property(&InputState::HoverState::getHovered),
			"all_hovered", sol::readonly_property(&InputState::HoverState::getAllHovered),
			"any_hovered", sol::readonly_property(&InputState::HoverState::anyHovered),
			"has_changed", sol::readonly_property(&InputState::HoverState::hasChanged)
		);

		sol::usertype<InputState::MouseState> mouse_state_t = lua.new_usertype<InputState::MouseState>("MouseState", 
			sol::constructors<InputState::MouseState()>(),
			"pos", sol::readonly_property(&InputState::MouseState::pos),
			"last_pos", sol::readonly_property(&InputState::MouseState::lastPos),
			"delta", sol::readonly_property(&InputState::MouseState::delta),
			"buttons", sol::readonly_property([&lua](InputState::MouseState& ms) {
				sol::table t = lua.create_table();
				for (int i = 0; i < 8; ++i) {
					t[i + 1] = ms.buttons[i];  // Lua uses 1-based indexing
				}
				return t;
			}),
			"dbl_buttons", sol::readonly_property([&lua](InputState::MouseState& ms) {
				sol::table t = lua.create_table();
				for (int i = 0; i < 8; ++i) {
					t[i + 1] = ms.dblButtons[i];  // Lua uses 1-based indexing
				}
				return t;
			}),
			"last_clicks", sol::readonly_property([&lua](InputState::MouseState& ms) {
				sol::table t = lua.create_table();
				for (int i = 0; i < 8; ++i) {
					t[i + 1] = ms.lastClicks[i];  // Lua uses 1-based indexing
				}
				return t;
			}),
			"cursor_radius", sol::readonly_property(&InputState::MouseState::getCursorRadius),
			"dbl_click_interval", sol::readonly_property(&InputState::MouseState::getDblClickInterval),
			"is_left_button", sol::readonly_property(&InputState::MouseState::isLeftButton),
			"is_right_button", sol::readonly_property(&InputState::MouseState::isRightButton),
			"is_middle_button", sol::readonly_property(&InputState::MouseState::isMiddleButton),
			"is_button_pressed", sol::readonly_property(&InputState::MouseState::isButtonPressed),
			"any_button_pressed", sol::readonly_property(&InputState::MouseState::anyButtonPressed)

		);

		sol::usertype<InputState> input_state_t = lua.new_usertype<InputState>("InputState", 
			sol::constructors<InputState(), InputState::HoverState()>(),
			"vertex", sol::readonly_property(&InputState::vertex),
			"edge", sol::readonly_property(&InputState::edge),
			"facet", sol::readonly_property(&InputState::facet),
			"cell", sol::readonly_property(&InputState::cell),
			"mesh", sol::readonly_property(&InputState::mesh),
			"mouse", sol::readonly_property(&InputState::mouse)
		);

		input_state_t.set_function("get_primitive_state", &InputState::getHoverState);



		sol::usertype<Colormap> colormap_t = lua.new_usertype<Colormap>("Colormap", 
			sol::constructors<Colormap()>(),
			"name", sol::readonly_property(&Colormap::name),
			"width", sol::readonly_property(&Colormap::width),
			"height", sol::readonly_property(&Colormap::height),
			"tex", sol::readonly_property(&Colormap::tex)
		);

		sol::usertype<NavigationPath> navigationPath_t = lua.new_usertype<NavigationPath>("NavigationPath",
			"get", &NavigationPath::get,
			"str", &NavigationPath::str,
			"starts_with", sol::overload(
				[](NavigationPath &self, std::vector<std::string> head) { self.startsWith(head); },
				[](NavigationPath &self, std::string head) { self.startsWith(head); }
			),
			sol::meta_function::to_string, [](NavigationPath& self) {
				return self.str();
			}//,
			// sol::meta_function::equal_to, [](const NavigationPath& self, const sol::object& other) {
			// 	if (other.is<std::string>()) {
			// 		return self.str() == other.as<std::string>();
			// 	} else if (other.is<NavigationPath>()) {
			// 		return self == other.as<NavigationPath>();
			// 	}
			// 	return false;
			// }
		);

		// General functions 
		lua.set_function("element_kind_to_string", &elementKindToString);
		lua.set_function("element_type_to_string", &elementTypeToString);

		// App bindings
		lua["app"] = &app;
		sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");

		app_type.set_function("quit", &IApp::quit);
		app_type.set_function("clear_scene", &IApp::clearScene);

		app_type.set_function("show_open_model_dialog", &IApp::showOpenModelDialog);
		app_type.set_function("show_save_model_dialog", &IApp::showSaveModelDialog);

		app_type.set_function("load_state", &IApp::loadState);
		app_type.set_function("save_state", &IApp::saveState);
		app_type.set_function("snapshot", &IApp::snapshot);
		app_type.set_function("load_snapshot", &IApp::loadSnapshot);
		app_type.set_function("list_snapshots", &IApp::listSnapshots);


		app_type["scene"] = sol::readonly_property(&IApp::getScene);

		sol::usertype<IScene> scene_t = lua.new_usertype<IScene>("IScene");

		scene_t.set_function("load_model", &IScene::loadModel);
		scene_t["models"] = sol::readonly_property(&IScene::getModels);
		scene_t["model"] = sol::readonly_property(&IScene::getCurrentModel);
		scene_t["hovered_model"] = sol::readonly_property(&IScene::getHoveredModel);
		scene_t["selected_model"] = sol::property(&IScene::getSelectedModel, &IScene::setSelectedModel);
		scene_t.set_function("focus", &IScene::focus);
		scene_t["views"] = sol::readonly_property(&IScene::getViews);
		
		scene_t["main_view"] = sol::readonly_property(&IScene::getMainView);
		scene_t["current_view"] = sol::readonly_property(&IScene::getCurrentView);
		scene_t["selected_view"] = sol::property(&IScene::getSelectedView, &IScene::setSelectedView);

		scene_t.set_function("add_view", &IScene::addView);


		auto sceneView_t = lua.new_usertype<ISceneView>(
			"ISceneView",
			"name", sol::readonly_property(&ISceneView::getName),
			"get_material", &ISceneView::getMaterial,
			"get_materials", &ISceneView::getMaterials,
			"get_state", &ISceneView::getState
		);

		auto modelState_t = lua.new_usertype<ModelState>(
			"ModelState",
			"get_selected_attr", [](ModelState &self, ColormapLayer layer) {
				return self.getSelectedAttr(layer) + 1;
			},
			"set_selected_attr", [](ModelState &self, int idx, ColormapLayer layer) {
				self.setSelectedAttr(idx - 1, layer);
			},
			"get_selected_colormap", [](ModelState &self, ColormapLayer layer) {
				return self.getSelectedColormap(layer) + 1;
			},
			"set_selected_colormap", [](ModelState &self, int idx, ColormapLayer layer) {
				self.setSelectedColormap(idx - 1, layer);
			},
			"get_layer_attr_name", &ModelState::getLayerAttrName,
			"set_layer_attr_name", &ModelState::setLayerAttrName,
			"set_layer", &ModelState::setLayer,
			"unset_layer", &ModelState::unsetLayer
		);

		auto materialInstance_t = lua.new_usertype<MaterialInstance>(
			"MaterialInstance",
			"visible", sol::property(&MaterialInstance::getVisible, &MaterialInstance::setVisible),
			"get_params", &MaterialInstance::getParams,
			sol::meta_function::index, &MaterialInstance::getParams
		);

		auto materialInstanceCollection_t = lua.new_usertype<MaterialInstanceCollection>(
			"MaterialInstanceCollection",
			"visible", sol::property(&MaterialInstanceCollection::getVisible, &MaterialInstanceCollection::setVisible),
			"get_material", &MaterialInstanceCollection::getMaterial,
			"set", &MaterialInstanceCollection::set,
			"all", &MaterialInstanceCollection::all,
			"any", &MaterialInstanceCollection::any,
			"first", &MaterialInstanceCollection::first,
			sol::meta_function::index, &MaterialInstanceCollection::getMaterial
		);

		sol::usertype<ShaderParams> shaderParams_t = lua.new_usertype<ShaderParams>(
			"ShaderParams",
			"get", &ShaderParams::get,
			"set", &ShaderParams::set,
			sol::meta_function::index, &ShaderParams::get,
			sol::meta_function::new_index, &ShaderParams::set
		);

		sol::usertype<LightParams> lightParams_t = lua.new_usertype<LightParams>(
			sol::base_classes, sol::bases<ShaderParams>(),
			"LightParams",
			"enabled", sol::property(&LightParams::getEnabled, &LightParams::setEnabled)
		);

		sol::usertype<PointSetParams> pointSetParams_t = lua.new_usertype<PointSetParams>(
			sol::base_classes, sol::bases<ShaderParams>(),
			"PointSetParams",
			"color", sol::property(&PointSetParams::getColor, &PointSetParams::setColor),
			"size", sol::property(&PointSetParams::getSize, &PointSetParams::setSize)
		);

		sol::usertype<MeshStyleParams> meshParams_t = lua.new_usertype<MeshStyleParams>(
			"MeshStyleParams",
			sol::base_classes, sol::bases<ShaderParams>(),
			"color", sol::property(&MeshStyleParams::getColor, &MeshStyleParams::setColor),
			"size", sol::property(&MeshStyleParams::getSize, &MeshStyleParams::setSize),
			"shrink", sol::property(&MeshStyleParams::getShrink, &MeshStyleParams::setShrink),
			"corner_visible", sol::property(&MeshStyleParams::getCornerVisible, &MeshStyleParams::setCornerVisible)
		);

		sol::usertype<HalfedgeParams> halfedgeParams_t = lua.new_usertype<HalfedgeParams>(
			"HalfedgeParams",
			sol::base_classes, sol::bases<ShaderParams>(),
			"inside_color", sol::property(&HalfedgeParams::getInsideColor, &HalfedgeParams::setInsideColor),
			"outside_color", sol::property(&HalfedgeParams::getOutsideColor, &HalfedgeParams::setOutsideColor),
			"thickness", sol::property(&HalfedgeParams::getThickness, &HalfedgeParams::setThickness),
			"spacing", sol::property(&HalfedgeParams::getSpacing, &HalfedgeParams::setSpacing),
			"padding", sol::property(&HalfedgeParams::getPadding, &HalfedgeParams::setPadding)
		);


		sol::usertype<ModelCollection> modelCollection_t = lua.new_usertype<ModelCollection>("ModelCollection",
			"add", &ModelCollection::add,
			"remove", &ModelCollection::remove,
			"get", sol::resolve<Model&(std::string)>(&ModelCollection::get),
			"all", sol::resolve<std::map<std::string, std::shared_ptr<Model>>&()>(&ModelCollection::get),
			"count", sol::readonly_property(&ModelCollection::count),
			"has", &ModelCollection::has,
			"any", sol::readonly_property(&ModelCollection::any),
			"clear", &ModelCollection::clear,
			"get_children_of", &ModelCollection::getChildrenOf,
			"get_by_index", &ModelCollection::getByIndex
		);

		modelCollection_t.set_function(sol::meta_function::pairs, [](sol::this_state L, ModelCollection& self) {
			auto it = self.begin();
			auto end = self.end();
			
			return sol::as_function([it, end](sol::this_state L) mutable {
				if (it == end) {
					// Signal end of iteration
					return std::make_tuple(
						sol::object(sol::lua_nil),
						sol::object(sol::lua_nil)
					);
				}
				auto key = it->first;
				auto value = it->second;
				std::advance(it, 1);

				return std::make_tuple(
					sol::object(L, sol::in_place, key),
					sol::object(L, sol::in_place, value)
				);
			});
		});
		
		modelCollection_t[sol::meta_function::index] = [](ModelCollection& self, const std::string name) -> const std::shared_ptr<Model> {
			// Check existence before return, elsewhere self[name] is added to map with nullptr
			// for example doing app.scene.models:missing_function() (equivalent to: app.scene.models["missing_function"]) will add a key "missing_function" to map with nullptr !!!
			return self.has(name) ? self[name] : nullptr;
		};


		scene_t["camera"] = sol::readonly_property(&IScene::getCurrentCamera);
		scene_t["cameras"] = sol::readonly_property(&IScene::getCameras);
		scene_t["selected_camera"] = sol::property([](IScene &self) {
			return self.getSelectedCamera();
		}, [](IScene &self, std::string selected) {
			self.setSelectedCamera(selected);
		});

		sol::usertype<CameraCollection> cameraCollection_t = lua.new_usertype<CameraCollection>("CameraCollection",
			"add", &CameraCollection::add,
			"remove", &CameraCollection::remove,
			"get", sol::resolve<Camera&(std::string)>(&CameraCollection::get),
			"all", sol::resolve<std::map<std::string, std::shared_ptr<Camera>>&()>(&CameraCollection::get),
			"count", sol::readonly_property(&CameraCollection::count),
			"has", &CameraCollection::has,
			"any", sol::readonly_property(&CameraCollection::any),
			"clear", &CameraCollection::clear
		);

		cameraCollection_t.set_function(sol::meta_function::pairs, [](sol::this_state L, CameraCollection& self) {
			auto it = self.begin();
			auto end = self.end();
			
			return sol::as_function([it, end](sol::this_state L) mutable {
				if (it == end) {
					return sol::object(L, sol::nil);
				}
				auto key = it->first;
				auto value = it->second;
				++it;
				return sol::object(L, sol::in_place, std::make_tuple(key, value));
			});
		});
		
		cameraCollection_t[sol::meta_function::index] = [](CameraCollection& self, const std::string name) -> const std::shared_ptr<Camera> {
			// Check existence before return, elsewhere self[name] is added to map with nullptr
			// for example doing app.scene.cameras:missing_function() (equivalent to: app.scene.models["missing_function"]) will add a key "missing_function" to map with nullptr !!!
			return self.has(name) ? self[name] : nullptr;
		};


		sol::usertype<RendererCollection> rendererCollection_t = lua.new_usertype<RendererCollection>("RendererCollection",
			"add", &RendererCollection::add,
			"remove", &RendererCollection::remove,
			"get", sol::resolve<Renderer&(std::string)>(&RendererCollection::get),
			"all", sol::resolve<std::map<std::string, std::shared_ptr<Renderer>>&()>(&RendererCollection::get),
			"count", sol::readonly_property(&RendererCollection::count),
			"has", &RendererCollection::has,
			"any", sol::readonly_property(&RendererCollection::any),
			"clear", &RendererCollection::clear
		);

		rendererCollection_t.set_function(sol::meta_function::pairs, [](sol::this_state L, RendererCollection& self) {
			auto it = self.begin();
			auto end = self.end();
			
			return sol::as_function([it, end](sol::this_state L) mutable {
				if (it == end) {
					return sol::object(L, sol::nil);
				}
				auto key = it->first;
				auto value = it->second;
				++it;
				return sol::object(L, sol::in_place, std::make_tuple(key, value));
			});
		});
		
		rendererCollection_t[sol::meta_function::index] = [](RendererCollection& self, const std::string name) -> const std::shared_ptr<Renderer> {
			// Check existence before return, elsewhere self[name] is added to map with nullptr
			// for example doing app.scene.renderers:missing_function() (equivalent to: app.scene.models["missing_function"]) will add a key "missing_function" to map with nullptr !!!
			return self.has(name) ? self[name] : nullptr;
		};


		scene_t["renderers"] = sol::readonly_property(&IScene::getRenderers);


		app_type["input_state"] = sol::readonly_property(&IApp::getInputState);

		app_type["colormaps"] = sol::readonly_property(&IApp::getColormaps);
		app_type.set_function("add_colormap", &IApp::addColormap);
		app_type.set_function("remove_colormap", &IApp::removeColormap);
		app_type.set_function("get_colormap", sol::overload(
			[](IApp &self, int idx) {
				return self.getColormap(idx);
			},
			[](IApp &self, const std::string name) {
				return self.getColormap(name);
			}
		));

		app_type.set_function("screenshot", [&app = app](const std::string& filename, std::optional<int> targetWidth, std::optional<int> targetHeight) {
				app.screenshot(filename, targetWidth.value_or(-1), targetHeight.value_or(-1));
			}
		);

		app_type.set_function("setCullMode", [&app = app](int mode) {
			app.setCullMode(mode);
		});

		// Navigation
		app_type["navigation_path"] = sol::property(
			&IApp::getNavigationPath,
			[](IApp& self, const sol::object& value) {
				if (value.is<std::string>()) {
					self.setNavigationPath(value.as<std::string>());
				} else if (value.is<std::vector<std::string>>()) {
					self.setNavigationPath(value.as<std::vector<std::string>>());
				}
			}
		);

		app_type.set_function("add_navigation_path", &IApp::addNavigationPath);
		app_type.set_function("top_navigation_path", &IApp::topNavigationPath);

		app_type["width"] = sol::readonly_property(&IApp::getWindowWidth);
		app_type["height"] = sol::readonly_property(&IApp::getWindowHeight);
		app_type["aspect_ratio"] = sol::readonly_property(&IApp::getAspectRatio);
		app_type["cull_mode"] = sol::writeonly_property(&IApp::setCullMode);
		app_type["cull"] = sol::property(&IApp::getCull, &IApp::setCull);

		app_type.set_function("pick_point", &IApp::pickPoint);
		app_type.set_function("pick_edge", &IApp::pickEdge);
		app_type.set_function("pick_mesh", &IApp::pick_mesh);
		app_type.set_function("pick_cells", &IApp::pick_cells);
		app_type.set_function("pick_facets", &IApp::pick_facets);
		app_type.set_function("pick_vertices", &IApp::pick_vertices);

		app_type["is_ui_hovered"] = sol::readonly_property(&IApp::isUIHovered);

		// Modules
		// app_type.set_function("load_module", &IApp::loadModule);

		app_type["is_debug"] = sol::readonly_property(&IApp::isDebug);
	}
}