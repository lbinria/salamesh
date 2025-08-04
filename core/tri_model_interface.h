#pragma once
#include "model.h"

struct TriModelInterface : public Model {

    constexpr ModelType getModelType() const override {
        return Model::ModelType::TRI;
    }

    
};
