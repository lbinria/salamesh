#include "hex_model.h"


void HexModel::push() {
	_hexRenderer.push();
	_pointSetRenderer.push();

	if (colorMode == Model::ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}