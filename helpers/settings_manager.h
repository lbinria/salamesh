#pragma once

#include <iostream>
// JSON !!
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

struct Settings {
	std::vector<std::string> modules;

	bool load(std::string path) {
		std::ifstream ifs(path);

		if (!ifs.is_open()) {
			std::cout << "Error while opening json file: " << path << std::endl;
			return false;
		}

		// Get the file size
		ifs.seekg(0, std::ios::end);
		std::streampos file_size = ifs.tellg();
		ifs.seekg(0, std::ios::beg);
		std::string content(file_size, '\0');
		ifs.read(&content[0], file_size);

		ifs.close();

		auto j_content = json::parse(content);

		modules = j_content["modules"].get<std::vector<std::string>>();

		return true;
	}
};