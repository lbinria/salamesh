#pragma once 

#include <filesystem>
#include <optional>

#include <fstream>

// JSON !!
#include <json.hpp>
using json = nlohmann::json;

struct Args {
	std::string settings_path = sl::exePath("settings.json");
	std::vector<std::filesystem::path> paths = {};

	bool parse(std::string content) {

		auto j_content = json::parse(content);
		
		if (j_content.contains("settings_path") && !j_content["settings_path"].is_null()) {
			settings_path = j_content["settings_path"].get<std::string>();
		}

		if (j_content.contains("models") && !j_content["models"].is_null() && j_content["models"].is_array()) {
			auto modelPaths = j_content["models"].get<std::vector<std::string>>();
			paths.insert(paths.end(), modelPaths.begin(), modelPaths.end());
		}

		if (j_content.contains("scripts") && !j_content["scripts"].is_null() && j_content["scripts"].is_array()) {
			auto scriptPaths = j_content["scripts"].get<std::vector<std::string>>();
			paths.insert(paths.end(), scriptPaths.begin(), scriptPaths.end());
		}

		return true;
	}
};
