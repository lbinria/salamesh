#pragma once

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

	PrimitiveState vertex;
	PrimitiveState edge;
	PrimitiveState facet;
	PrimitiveState cell;

};