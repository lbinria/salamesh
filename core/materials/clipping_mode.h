#pragma once
enum ClippingMode {
	CELL = 0,
	STD = 1
};

static constexpr const char* clippingModeStrings[2] = {"Cell", "Std"};

constexpr std::array<std::string_view, 2> getClippingModeStrings() {
	return {clippingModeStrings[0], clippingModeStrings[1]};
}