#pragma once 
#include <string>
#include "element.h"

enum Layer {
		COLORMAP,
		HIGHLIGHT,
		FILTER
	};

static std::string layerToString(Layer l) {
	switch (l) {
		case Layer::COLORMAP: return "colormap";
		case Layer::HIGHLIGHT: return "highlight";
		case Layer::FILTER: return "filter";
		default: return "unknown";
	}
}

static std::string defaultAttrName(Layer l) {
	switch (l) {
		case Layer::COLORMAP: return "_colormap_0";
		case Layer::HIGHLIGHT: return "_highlight";
		case Layer::FILTER: return "_filter";
		default: return "unknown";
	}
}