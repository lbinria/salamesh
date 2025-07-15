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

			camera_t.set_function("LookAt", [&app = app](ArcBallCamera &camera, float x, float y, float z) {
				app.getCamera().LookAt(glm::vec3(x, y, z));
			});

			camera_t.set_function("GetFovAndScreen", [&app = app]() {
				auto fov_and_screen = app.getCamera().GetFovAndScreen();
				return std::make_tuple(fov_and_screen.x, fov_and_screen.y, fov_and_screen.z);
			});

			camera_t.set_function("SetFov", &ArcBallCamera::SetFov);
			camera_t.set_function("MoveRight", &ArcBallCamera::MoveRight);
			camera_t.set_function("MoveUp", &ArcBallCamera::MoveUp);
			camera_t.set_function("MoveForward", &ArcBallCamera::MoveForward);
			camera_t.set_function("reset_zoom", &ArcBallCamera::reset_zoom);

			camera_t["position"] = sol::property(
				&ArcBallCamera::GetEye,
				&ArcBallCamera::SetEye
			);

			camera_t["look_at"] = sol::property(
				&ArcBallCamera::GetLookAt,
				&ArcBallCamera::LookAt
			);

		}

	};

}