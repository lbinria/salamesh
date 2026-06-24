#include "geometry.h"

void Geometry::saveState(json &j, const std::string filename) {

	std::filesystem::path p(filename);
	auto dirPath = p.parent_path().string();

	// Save current mesh state into a file
	auto guid = sl::generateGuid();
	auto geometryFilename = guid + ".geogram";
	auto filepath = std::filesystem::path(dirPath) / geometryFilename;
	
	if (!saveAs(filepath.string())) {
		std::cerr << "Unable to save state at " << filename << std::endl;
		return;
	}

	j["filename"] = filename;	
}

void Geometry::loadState(json &j, const std::string filename) {

	std::string geometryFilename = j["filename"];
		auto geometryPath = 
			std::filesystem::path(filename).remove_filename() / 
			std::filesystem::path(geometryFilename);

	if (!load(geometryPath.string())) {
		std::cerr << "Unable to load state at " << filename << std::endl;
		return;
	}
}
