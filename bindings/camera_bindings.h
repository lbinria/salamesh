#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/arcball_camera.h"
#include "../core/descent_camera.h"

namespace bindings {

	struct CameraBindings {

		static void load_bindings(sol::state &lua, sol::usertype<IApp> app_type, IApp &app) {

			sol::usertype<DescentCamera> camera_t = lua.new_usertype<DescentCamera>("DescentCamera");
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);

			camera_t.set_function("move_right", &DescentCamera::moveRight);
			camera_t.set_function("move_up", &DescentCamera::moveUp);
			camera_t.set_function("move_forward", &DescentCamera::moveForward);
			camera_t.set_function("zoom", &DescentCamera::zoom);
			camera_t.set_function("reset_zoom", &DescentCamera::resetZoom);

			camera_t["position"] = sol::property(
				&DescentCamera::getEye,
				&DescentCamera::setEye
			);

			camera_t["look_at"] = sol::property(
				&DescentCamera::getLookAt,
				&DescentCamera::lookAt
			);

			camera_t["fov"] = sol::property(
				&DescentCamera::getFov,
				&DescentCamera::setFov
			);

			camera_t["fov_and_screen"] = sol::readonly_property(
				&DescentCamera::getFovAndScreen
			);

			camera_t["lock"] = sol::property(
				&DescentCamera::isLocked,
				&DescentCamera::setLock
			);

		}

	};

}