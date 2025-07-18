#pragma once
#include <glm/glm.hpp>

struct InputState {

	struct PrimitiveState {

		long get_hovered() {
			return hovered;
		}

		void set_hovered(long idx) {
			last_hovered = hovered;
			hovered = idx;
		}

		bool is_hovered() {
			return hovered >= 0;
		}

		bool is_changed() {
			return last_hovered != hovered;
		}

		private:
		long last_hovered;
		long hovered;
	};

	struct MouseState {
		glm::vec2 pos;
		glm::vec2 lastPos;
		bool buttons[8] = {false, false, false, false, false, false, false, false};
		
		bool isLeftButton() const {
			return buttons[0];
		}

		bool isRightButton() const {
			return buttons[1];
		}

		bool isMiddleButton() const {
			return buttons[2];
		}

		// bool left = false;
		// bool right = false;
		bool dblLeft = false;
		glm::vec2 scrollDelta = {0, 0};
		int cursor_radius = 1;
	};

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;

	MouseState mouse;

};