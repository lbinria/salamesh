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

	struct PrimitiveState2 {

		std::vector<long> get_all_hovered() {
			return hovered;
		}

		long get_hovered() {
			// Get the hovered element at the mouse pos
			if (hovered.empty()) {
				return -1;
			}
			// Get middle element
			int idx = hovered.size() / 2;
			return hovered[idx];
		}

		void set_hovered(const std::vector<long>& idxs) {
			last_hovered = hovered;
			hovered = idxs;
		}

		bool is_hovered() {
			return !hovered.empty();
		}

		bool has_changed() {
			return last_hovered != hovered;
		}

		private:
		std::vector<long> last_hovered;
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
		glm::vec2 scrollDelta = {0, 0};
		int cursor_radius = 1;
	};

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;
	PrimitiveState2 facets;
	PrimitiveState2 cells;

	MouseState mouse;

};