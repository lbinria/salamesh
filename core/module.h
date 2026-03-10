#pragma once

#include "script.h"
#include "renderers/renderer_info.h"


struct Module {

	std::unique_ptr<Script> script;
	std::vector<std::unique_ptr<RendererInfo>> rInfos;

};