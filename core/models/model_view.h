#pragma once
#include "../renderers/renderer_view.h"
#include "../models/model.h"
#include "../renderers/renderer.h"

#include <map>
#include <string>

struct ModelView {

	ModelView(std::map<std::string, std::shared_ptr<RendererView>> &rendererViews) : rendererViews(rendererViews) {}

	ModelView getDefaultView(Model &m) {
		std::map<std::string, std::shared_ptr<RendererView>> rv;
		for (auto &[k, r] : m.getRenderers()) {
			rv[k] = std::move(r->getDefaultView());
		}
		return ModelView(rv);
	}

	// TODO to unique_ptr
	std::map<std::string, std::shared_ptr<RendererView>> rendererViews;

};