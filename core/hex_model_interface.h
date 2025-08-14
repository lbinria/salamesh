#pragma once
#include "model.h"

struct IHexModel : public Model {

	virtual Hexahedra& getHexahedra() = 0;
	virtual VolumeAttributes& getVolumeAttributes() = 0;

};