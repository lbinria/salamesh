#include "mesh.h"

void Mesh::saveState(json &j, const std::string filename) {

	std::filesystem::path p(filename);
	auto dirPath = p.parent_path().string();

	// Save current mesh state into a file
	auto guid = sl::generateGuid();
	auto meshFilename = guid + ".geogram";
	auto filepath = std::filesystem::path(dirPath) / meshFilename;
	
	if (!saveAs(filepath.string())) {
		std::cerr << "Unable to save state at " << filename << std::endl;
		return;
	}

	j["filename"] = filename;	
}

void Mesh::loadState(json &j, const std::string filename) {

	std::string meshFilename = j["filename"];
		auto meshPath = 
			std::filesystem::path(filename).remove_filename() / 
			std::filesystem::path(meshFilename);

	if (!load(meshPath.string())) {
		std::cerr << "Unable to load state at " << filename << std::endl;
		return;
	}
}
