#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace bindings {

	struct FsBindings {

		static std::vector<std::string> listDirectory(std::string dirPath, std::string ext = "") {
			std::vector<std::string> filenames;
			try {
				for (auto const& dir_entry : fs::directory_iterator(dirPath)) {
					if (!fs::is_regular_file(dir_entry.path()) || 
						ext != "" && dir_entry.path().extension() != ext)
						continue;

					// std::string name = dir_entry.path().stem();
					filenames.push_back(dir_entry.path().filename());
				}
			}
			catch (const fs::filesystem_error & ex) {
				// throw std::runtime_error("Error occurred during loading latest snapshot. " + ex.what());
				throw std::runtime_error("Error occurred during loading latest snapshot.");
			}
			
			return filenames;
		}

		static void loadBindings(sol::state &lua) {

			lua.set_function("list_directory", &listDirectory);

		}

	};

}