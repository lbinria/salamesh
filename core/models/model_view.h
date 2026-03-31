#pragma once
#include "../renderers/renderer_view.h"
#include "../renderers/renderer.h"

#include <map>
#include <string>

struct ModelView {

	ModelView(std::map<std::string, std::shared_ptr<RendererView>> &rendererViews) : rendererViews(rendererViews) {}



	// TODO to unique_ptr
	std::map<std::string, std::shared_ptr<RendererView>> rendererViews;

};