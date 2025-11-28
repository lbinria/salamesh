#pragma once

#include <chrono>
#include <vector>
#include "../include/glm/glm.hpp"

struct InputState {

	struct PrimitiveState {

		std::vector<long> getAllHovered() {
			return hovered;
		}

		// Keep attention that returned element is arbitrary 
		// when the cursor radius is greater than 1
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
		glm::vec2 delta;
		bool buttons[8] = {false, false, false, false, false, false, false, false};
		bool dblButtons[8] = {false, false, false, false, false, false, false, false};
		std::chrono::steady_clock::time_point lastClicks[8];
		
		bool isLeftButton() const {
			return buttons[0];
		}

		bool isRightButton() const {
			return buttons[1];
		}

		bool isMiddleButton() const {
			return buttons[2];
		}

		bool isButtonPressed(int button) const {
			if (button < 0 || button >= 8) return false;
			return buttons[button];
		}

		bool anyButtonPressed() const {
			for (bool b : buttons) {
				if (b) return true;
			}
			return false;
		}

		int getCursorRadius() const {
			return cursorRadius;
		}

		void setCursorRadius(int radius) {
			cursorRadius = std::clamp(radius, 0, 50);
		}

		int getDblClickInterval() const {
			return dblClickInterval;
		}

		glm::vec2 scrollDelta = {0, 0};
		
		private:
		int cursorRadius = 0;

		// std::chrono::steady_clock::time_point lastClick;
		// bool dblLeft = false;
		int dblClickInterval = 200 /*ms*/;
	};

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;
	PrimitiveState mesh;

	MouseState mouse;

	// Get mouse state of a given primitive (VERTEX, EDGE, FACET, CELL...)
	PrimitiveState& getPrimitiveState(ElementKind kind) {
		switch (kind)
		{
		case ElementKind::POINTS_ELT:
			return vertex;
		case ElementKind::EDGES:
			return edge;
		case ElementKind::FACETS:
			return facet;
		case ElementKind::CELLS:
			return cell;
		case ElementKind::CELL_FACETS:
			return facet;
		default:
			throw new std::runtime_error("Unsupported primitive state: " + elementKindToString(kind));
		}
	}

};