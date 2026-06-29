#pragma once

#include "geometry_buffer.h"
#include "material.h"

struct ViewComponent {
	
	ViewComponent(std::string name) :  
	_name(name) {
		_index = maxIndex;
		++maxIndex;
	}

	int getIndex() const {
		return _index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	bool isVisible() const {
		return _visible;
	}

	private:
	std::string _name;
	// GeometryBuffer _geometryBuffer;
	Material _material;
	bool _visible = true;

	static inline int maxIndex = 0;
	int _index;

};