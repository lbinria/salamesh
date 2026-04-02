#pragma once
#include "../renderers/renderer_view.h"
#include "../renderers/renderer.h"

#include <map>
#include <string>
#include "model.h"

struct ModelView {

	ModelView(std::string viewName, Model &m) : viewName(viewName), model(m) {

	}

	PointSetRendererView& getPoints() {
		return *std::static_pointer_cast<PointSetRendererView>(model.getRenderer("point_renderer")->getView(viewName));
	}

	std::shared_ptr<MeshRendererView> getMesh() const {
		if(model.hasRenderer("mesh_renderer"))
			return std::static_pointer_cast<MeshRendererView>(model.getRenderer("mesh_renderer")->getView(viewName));
		
		return nullptr;
	}

	bool getLightEnabled() const {
		return isLightEnabled;
	}

	void setLightEnabled(bool enabled) {
		for (auto const &[k, r] : model.getRenderers())
			r->getView(viewName)->setLightEnabled(enabled);

		isLightEnabled = enabled;
	}

	bool visible;

	private:
	std::string viewName;
	Model &model;
	bool isLightEnabled;

};