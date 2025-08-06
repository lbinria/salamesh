#pragma once
// #include <glm/glm.hpp>
#include "../include/glm/glm.hpp"

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
			// Get first element
			return hovered[0];
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
		bool _isDragging = false;
		
		bool isLeftButton() const {
			return buttons[0];
		}

		bool isRightButton() const {
			return buttons[1];
		}

		bool isMiddleButton() const {
			return buttons[2];
		}

		int getCursorRadius() const {
			return cursorRadius;
		}

		void setCursorRadius(int radius) {
			cursorRadius = std::clamp(radius, 0, 50);
		}

		bool isDragging() const {
			return _isDragging;
		}

		glm::vec2 scrollDelta = {0, 0};
		
		private:
		int cursorRadius = 0;

		std::chrono::steady_clock::time_point lastClick;
		bool dblLeft = false;
		int dblClickInterval = 300 /*ms*/;
	};

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;

	MouseState mouse;

};