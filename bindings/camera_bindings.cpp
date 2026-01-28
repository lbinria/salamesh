#include "camera_bindings.h"

#include "../core/cameras/camera.h"

namespace bindings {

	void CameraBindings::loadBindings(sol::state &lua, IApp &app) {
		
		sol::usertype<Camera> camera_t = lua.new_usertype<Camera>("Camera");

		camera_t.set_function("move_right", &Camera::moveRight);
		camera_t.set_function("move_up", &Camera::moveUp);
		camera_t.set_function("move_forward", &Camera::moveForward);
		camera_t.set_function("zoom", &Camera::zoom);
		camera_t.set_function("reset_zoom", &Camera::resetZoom);
		
		camera_t.set_function("look_at_box", [](Camera &self, sol::table box) { 
			auto min = box.get<glm::vec3>(1);
			auto max = box.get<glm::vec3>(2);
			self.lookAtBox(std::make_tuple(min, max)); 
		});

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

		camera_t["zoom"] = sol::property(
			&Camera::getZoom,
			&Camera::setZoom
		);

		camera_t["lock"] = sol::property(
			&Camera::isLocked,
			&Camera::setLock
		);

		camera_t["near_plane"] = sol::property(
			&Camera::getNearPlane,
			&Camera::setNearPlane
		);

		camera_t["far_plane"] = sol::property(
			&Camera::getFarPlane,
			&Camera::setFarPlane
		);
	}

}