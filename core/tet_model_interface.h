#pragma once
#include "model.h"

struct ITetModel : public Model {

	using Model::Model;

	virtual Tetrahedra& getTetrahedra() = 0;
	virtual VolumeAttributes& getVolumeAttributes() = 0;

    virtual void setFacetHighlight(int idx, float highlight) = 0;
    virtual void setFacetHighlight(std::vector<float> highlights) = 0;

};