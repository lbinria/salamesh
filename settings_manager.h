// JSON !!
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

struct Settings {
	std::vector<std::string> modules;
};

static bool load(std::string &path, Settings &settings) {

	std::ifstream ifs(path);

	if (!ifs.is_open()) {
		// std::cout << "Error while opening json file." << std::endl;
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

	std::vector<json> j_modules = j_content["modules"].get<std::vector<json>>();

	std::transform(j_modules.begin(), j_modules.end(), settings.modules.begin(), [](auto &j) {
		return j.get<std::string>();
	});
}