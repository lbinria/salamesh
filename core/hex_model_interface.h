#pragma once
#include "model.h"

struct IHexModel : public Model {

	using Model::Model;

	virtual Hexahedra& getHexahedra() = 0;
	virtual VolumeAttributes& getVolumeAttributes() = 0;

    virtual void setFacetHighlight(int idx, float highlight) = 0;
    virtual void setFacetHighlight(std::vector<float> highlights) = 0;

};