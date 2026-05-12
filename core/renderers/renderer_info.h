#pragma once

#include <string>
#include <functional>

struct RendererInfo {
	std::string type;
	std::function<std::unique_ptr<Material>(std::string)> instanciatorFunc;
};