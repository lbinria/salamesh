#pragma once
#include "../renderers/renderer_view.h"
#include "../renderers/renderer.h"

#include <map>
#include <string>
#include "model.h"

struct ModelView {

	ModelView(Model &m, std::map<std::string, std::shared_ptr<RendererView>> &rendererViews) : model(m), rendererViews(rendererViews) {}

	PointSetRendererView& getPoints() {
		return *static_cast<PointSetRendererView*>(rendererViews.at("point_renderer").get());
	}

	std::shared_ptr<MeshRendererView> getMesh() const {
		if(rendererViews.contains("mesh_renderer"))
			return std::static_pointer_cast<MeshRendererView>(rendererViews.at("mesh_renderer"));
		
		return nullptr;
	}

	bool getLightEnabled() const {
		return isLightEnabled;
	}

	void setLightEnabled(bool enabled) {
		for (auto const &[k, rv] : rendererViews)
			rv->setLightEnabled(enabled);

		isLightEnabled = enabled;
	}

	// TODO to unique_ptr
	std::map<std::string, std::shared_ptr<RendererView>> rendererViews;

	bool visible;

	private:
	Model &model;
	bool isLightEnabled;

};