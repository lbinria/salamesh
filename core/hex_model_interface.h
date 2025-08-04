#pragma once
#include "model.h"

struct HexModelInterface : public Model {

    constexpr ModelType getModelType() const override {
        return Model::ModelType::HEX;
    }

    
};
