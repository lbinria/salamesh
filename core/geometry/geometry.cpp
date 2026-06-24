#include "geometry.h"

void Geometry::saveState(json &j, const std::string dirPath) {
	// Save current mesh state into a file
	auto guid = sl::generateGuid();
	auto filename = guid + ".geogram";
	auto filepath = std::filesystem::path(dirPath) / filename;
	
	if (!saveAs(filepath.string())) {
		std::cerr << "Unable to save state at " << filepath << std::endl;
		return;
	}

	j["path"] = filename;	
}