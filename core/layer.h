#pragma once 
#include <string>
#include "element.h"

enum Layer {
		COLORMAP,
		HIGHLIGHT,
		FILTER
	};

static std::string layerToString(Layer t) {
	switch (t) {
		case Layer::COLORMAP: return "colormap";
		case Layer::HIGHLIGHT: return "highlight";
		case Layer::FILTER: return "filter";
		default: return "unknown";
	}
}