#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/camera.h"

namespace bindings {

	struct CameraBindings {

		static void load_bindings(sol::state &lua, sol::usertype<IApp> app_type, IApp &app) {

			sol::usertype<Camera> camera_t = lua.new_usertype<Camera>("Camera");
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);

			camera_t.set_function("move_right", &Camera::moveRight);
			camera_t.set_function("move_up", &Camera::moveUp);
			camera_t.set_function("move_forward", &Camera::moveForward);
			camera_t.set_function("zoom", &Camera::zoom);
			camera_t.set_function("reset_zoom", &Camera::resetZoom);

			camera_t["name"] = sol::property(
				&Camera::getName,
				&Camera::setName
			);

			camera_t["position"] = sol::property(
				&Camera::getEye,
				&Camera::setEye
			);

			camera_t["look_at"] = sol::property(
				&Camera::getLookAt,
				&Camera::lookAt
			);

			camera_t["fov"] = sol::property(
				&Camera::getFov,
				&Camera::setFov
			);

			camera_t["fov_and_screen"] = sol::readonly_property(
				&Camera::getFovAndScreen
			);

			camera_t["lock"] = sol::property(
				&Camera::isLocked,
				&Camera::setLock
			);

		}

	};

}