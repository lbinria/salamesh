#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/arcball_camera.h"

namespace bindings {

	struct CameraBindings {

		static void load_bindings(sol::state &lua, sol::usertype<IApp> app_type, IApp &app) {

			sol::usertype<ArcBallCamera> camera_t = lua.new_usertype<ArcBallCamera>("ArcBallCamera");
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);

			camera_t.set_function("move_right", &ArcBallCamera::moveRight);
			camera_t.set_function("move_up", &ArcBallCamera::moveUp);
			camera_t.set_function("move_forward", &ArcBallCamera::moveForward);
			camera_t.set_function("zoom", &ArcBallCamera::zoom);
			camera_t.set_function("reset_zoom", &ArcBallCamera::resetZoom);

			camera_t["position"] = sol::property(
				&ArcBallCamera::getEye,
				&ArcBallCamera::setEye
			);

			camera_t["look_at"] = sol::property(
				&ArcBallCamera::getLookAt,
				&ArcBallCamera::lookAt
			);

			camera_t["fov"] = sol::property(
				&ArcBallCamera::getFov,
				&ArcBallCamera::setFov
			);

			camera_t["fov_and_screen"] = sol::readonly_property(
				&ArcBallCamera::getFovAndScreen
			);

			camera_t["lock"] = sol::property(
				&ArcBallCamera::isLocked,
				&ArcBallCamera::setLock
			);

		}

	};

}