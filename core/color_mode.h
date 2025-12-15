#pragma once
#include <string>

// Color modes:
// Solid color display, or attribute display
enum ColorMode {
	COLOR,
	ATTRIBUTE,
};

static std::string colorModeToString(ColorMode t) {
	switch (t) {
		case ColorMode::COLOR: return "color";
		case ColorMode::ATTRIBUTE: return "attribute";
		default: return "unknown";
	}
}