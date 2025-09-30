#pragma once 

// JSON !!
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

struct Args {
	std::string settings_path = "settings.json";
	std::vector<std::string> models = {};
	std::vector<std::string> scripts = {};
	// std::vector<std::string> models = {"assets/catorus_hex_attr.geogram"};
	// std::vector<std::string> models = {"assets/joint.geogram"};
	// std::vector<std::string> models = {};

	bool parse(std::string content) {

		auto j_content = json::parse(content);
		
		if (j_content.contains("settings_path") && !j_content["settings_path"].is_null()) {
			settings_path = j_content["settings_path"].get<std::string>();
		}

		if (j_content.contains("models") && !j_content["models"].is_null() && j_content["models"].is_array()) {
			models = j_content["models"].get<std::vector<std::string>>();
		}

		if (j_content.contains("scripts") && !j_content["scripts"].is_null() && j_content["scripts"].is_array()) {
			scripts = j_content["scripts"].get<std::vector<std::string>>();
		}

		return true;
	}
};
