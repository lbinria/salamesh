#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/element.h"

namespace bindings {

	struct AppBindings {

		static sol::usertype<IApp> loadBindings(sol::state &lua, IApp &app) {

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

			// Input state binding
			sol::usertype<InputState::PrimitiveState> primitive_state_t = lua.new_usertype<InputState::PrimitiveState>("PrimitiveState", 
				sol::constructors<InputState::PrimitiveState()>(),
				"hovered", sol::property(
					&InputState::PrimitiveState::getHovered,
					&InputState::PrimitiveState::setHovered
				),
				"all_hovered", sol::readonly_property(&InputState::PrimitiveState::getAllHovered),
				"any_hovered", sol::readonly_property(&InputState::PrimitiveState::anyHovered),
				"has_changed", sol::readonly_property(&InputState::PrimitiveState::hasChanged)
			);

			sol::usertype<InputState> input_state_t = lua.new_usertype<InputState>("InputState", 
				sol::constructors<InputState(), InputState::PrimitiveState()>(),
				"vertex", sol::readonly_property(&InputState::vertex),
				"edge", sol::readonly_property(&InputState::edge),
				"facet", sol::readonly_property(&InputState::facet),
				"cell", sol::readonly_property(&InputState::cell),
				"mesh", sol::readonly_property(&InputState::mesh)
			);

			// General functions 
			lua.set_function("elementKindToString", &elementKindToString);
			lua.set_function("elementTypeToString", &elementTypeToString);

			sol::usertype<Colormap> colormap_t = lua.new_usertype<Colormap>("Colormap", 
				sol::constructors<Colormap()>(),
				"name", sol::readonly_property(&Colormap::name),
				"width", sol::readonly_property(&Colormap::width),
				"height", sol::readonly_property(&Colormap::height),
				"tex", sol::readonly_property(&Colormap::tex)
			);

			// App bindings
			lua["app"] = &app;
			sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");

			app_type.set_function("clear_scene", &IApp::clearScene);
			app_type.set_function("quit", &IApp::quit);

			app_type.set_function("show_open_model_dialog", &IApp::showOpenModelDialog);
			app_type.set_function("show_save_model_dialog", &IApp::showSaveModelDialog);

			app_type.set_function("load_state", &IApp::loadState);
			app_type.set_function("save_state", &IApp::saveState);
			app_type.set_function("snapshot", &IApp::snapshot);
			app_type.set_function("load_snapshot", &IApp::loadSnapshot);
			app_type.set_function("list_snapshots", &IApp::listSnapshots);

			app_type["navigation_path"] = sol::property(&IApp::getNavigationPath, &IApp::setNavigationPath);
			app_type["navigation_path_string"] = sol::readonly_property(&IApp::getNavigationPathString);

			app_type["count_models"] = sol::readonly_property(&IApp::countModels);
			app_type["has_models"] = sol::readonly_property(&IApp::hasModels);
			app_type["models"] = sol::readonly_property(&IApp::getModels);
			app_type.set_function("getChildrenOf", &IApp::getChildrenOf);
			app_type["model"] = sol::readonly_property(&IApp::getCurrentModel);

			app_type["cameras"] = sol::readonly_property(&IApp::getCameras);
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);
			
			app_type["selected_model"] = sol::property([](IApp &self) {
				return self.getSelectedModel() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedModel(selected - 1);
			});

			app_type["selected_camera"] = sol::property([](IApp &self) {
				return self.getSelectedCamera() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedCamera(selected - 1);
			});

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

			app_type.set_function("load_model", &IApp::loadModel);
			
			app_type.set_function("add_model", [](IApp &self, std::string name, int type) {
				return self.addModel(name, (ModelType)type) + 1;
			});

			app_type.set_function("remove_model", sol::overload(
				[](IApp &self, int idx) {
					self.removeModel(idx);
				},
				[](IApp &self, std::string name) -> bool {
					return self.removeModel(name);
				}
			));

			app_type.set_function("get_model_by_name", &IApp::getModelByName);

			app_type.set_function("get_index_of_model", [](IApp &self, std::string name) {
				int idx = self.getIndexOfModel(name);
				return idx != -1 ? idx + 1 : -1;
			});
			
			app_type.set_function("screenshot", [&app = app](const std::string& filename) {
				app.screenshot(filename);
			});

			app_type.set_function("setCullMode", [&app = app](int mode) {
				app.setCullMode(mode);
			});

			app_type["cull_mode"] = sol::writeonly_property(&IApp::setCullMode);
			app_type["cull"] = sol::property(&IApp::getCull, &IApp::setCull);

			return app_type;
		}

	};

}