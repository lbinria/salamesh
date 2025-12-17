#pragma once 
#include <string>
#include "element.h"

enum Layer {
		COLORMAP_0,
		COLORMAP_1,
		COLORMAP_2,
		HIGHLIGHT,
		FILTER
	};

static std::string layerToString(Layer l) {
	switch (l) {
		case Layer::COLORMAP_0: return "colormap_0";
		case Layer::COLORMAP_1: return "colormap_1";
		case Layer::COLORMAP_2: return "colormap_2";
		case Layer::HIGHLIGHT: return "highlight";
		case Layer::FILTER: return "filter";
		default: return "unknown";
	}
}

static std::string defaultAttrName(Layer l) {
	switch (l) {
		case Layer::COLORMAP_0: return "_colormap_0";
		case Layer::COLORMAP_1: return "_colormap_1";
		case Layer::COLORMAP_2: return "_colormap_2";
		case Layer::HIGHLIGHT: return "_highlight";
		case Layer::FILTER: return "_filter";
		default: return "unknown";
	}
}