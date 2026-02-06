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

		camera_t.set_function("move", [](Camera &self, sol::table oldPos, sol::table newPos) {
			glm::vec2 old{oldPos.get<float>(1), oldPos.get<float>(2)};
			glm::vec2 newp{newPos.get<float>(1), newPos.get<float>(2)};
			self.move(old, newp);
		});

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

		camera_t["view_matrix"] = sol::readonly_property([&lua](Camera &self) {
			auto matrix = self.getViewMatrix();
			sol::table t = lua.create_table();
			for (int i = 0; i < 4; ++i) {
				sol::table row = lua.create_table();
				for (int j = 0; j < 4; ++j) {
					row[j + 1] = matrix[i][j];  // Lua uses 1-based indexing
				}
				t[i + 1] = row;
			}
			return t;
		});

		camera_t["projection_matrix"] = sol::readonly_property([&lua](Camera &self) {
			auto matrix = self.getProjectionMatrix();
			sol::table t = lua.create_table();
			for (int i = 0; i < 4; ++i) {
				sol::table row = lua.create_table();
				for (int j = 0; j < 4; ++j) {
					row[j + 1] = matrix[i][j];  // Lua uses 1-based indexing
				}
				t[i + 1] = row;
			}
			return t;
		});

		camera_t["right_vector"] = sol::readonly_property([&lua](Camera &self) {
			auto vec = self.getRightVector();
			sol::table t = lua.create_table();
			t[1] = vec.x;
			t[2] = vec.y;
			t[3] = vec.z;
			return t;
		});

		camera_t["up_vector"] = sol::readonly_property([&lua](Camera &self) {
			auto vec = self.getUpVector();
			sol::table t = lua.create_table();
			t[1] = vec.x;
			t[2] = vec.y;
			t[3] = vec.z;
			return t;
		});

		camera_t["view_dir"] = sol::readonly_property([&lua](Camera &self) {
			auto vec = self.getViewDir();
			sol::table t = lua.create_table();
			t[1] = vec.x;
			t[2] = vec.y;
			t[3] = vec.z;
			return t;
		});

		camera_t["near_plane"] = sol::property(
			&Camera::getNearPlane,
			&Camera::setNearPlane
		);

		camera_t["far_plane"] = sol::property(
			&Camera::getFarPlane,
			&Camera::setFarPlane
		);

		camera_t.set_function("save_state", [](Camera &self) {
			json j;
			self.saveState(j);
			return j;
		});

		camera_t.set_function("load_state", [](Camera &self, json &j) {
			self.loadState(j);
		});

		camera_t.set_function("copy", [](Camera &self, Camera &other, sol::table box) {
			auto min = box.get<glm::vec3>(1);
			auto max = box.get<glm::vec3>(2);
			self.copy(other, std::make_tuple(min, max));
		});

		camera_t["type"] = sol::readonly_property(&Camera::getType);
	}

}