#pragma once
#include <glm/glm.hpp>

struct InputState {

	struct PrimitiveState {

		std::vector<long> getAllHovered() {
			return hovered;
		}

		long getHovered() {
			// Get the hovered element at the mouse pos
			if (hovered.empty()) {
				return -1;
			}
			// Get middle element
			int idx = hovered.size() / 2;
			return hovered[idx];
		}

		void setHovered(const std::vector<long>& idxs) {
			lastHovered = hovered;
			hovered = idxs;
		}
		// TODO rename to anyHovered ?
		bool anyHovered() {
			return !hovered.empty();
		}

		bool hasChanged() {
			return lastHovered != hovered;
		}

		private:
		std::vector<long> lastHovered;
		std::vector<long> hovered;

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

		bool dblLeft = false;
		int _dbl_click_interval = 300 /*ms*/;
		std::chrono::steady_clock::time_point lastClick;

		glm::vec2 scrollDelta = {0, 0};
		int cursor_radius = 1; // TODO rename
	};

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;

	MouseState mouse;

};