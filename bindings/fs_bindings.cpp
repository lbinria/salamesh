#include "fs_bindings.h"

#include <filesystem>
#include "../core/graphic_api.h"
#include "../core/helpers.h"

namespace bindings {

	void FsBindings::loadBindings(sol::state &lua, IApp &app) {

		// static std::vector<std::string> listDirectory(std::string dirPath, std::string ext = "") {
		// 	if (!fs::exists(dirPath))
		// 		return {};
			
		// 	std::vector<std::string> filenames;
		// 	try {
		// 		for (auto const& dir_entry : fs::directory_iterator(dirPath)) {
		// 			if (!fs::is_regular_file(dir_entry.path()) || 
		// 				ext != "" && dir_entry.path().extension() != ext)
		// 				continue;

		// 			// std::string name = dir_entry.path().stem();
		// 			filenames.push_back(fs::absolute(dir_entry.path()).string());
		// 		}
		// 	}
		// 	catch (const fs::filesystem_error & ex) {
		// 		// throw std::runtime_error("Error occurred during loading latest snapshot. " + ex.what());
		// 		throw std::runtime_error("Error occurred during loading latest snapshot.");
		// 	}

		// 	return filenames;
		// }

		// static std::tuple<unsigned int, int, int, int> loadImage(std::string path) {
		// 	unsigned int tex;
		// 	int w, h, c;
		// 	sl::load_texture_2d(path, tex, w, h, c);
		// 	return std::make_tuple(tex, w, h, c);
		// }


		// static void loadBindings(sol::state &lua) {

		// 	lua.set_function("list_directory", &sl::listDirectory);
		// 	lua.set_function("load_image", &loadImage);
		// }

	}
}