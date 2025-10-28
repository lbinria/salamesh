#include "my_app.h"
#include "helpers/args_manager.h"

int main(int argc, char** argv) {

	Args args;

	bool isJsonNext = false;
	std::vector<std::filesystem::path> paths;
	for (int i = 1; i < argc; ++i) {
		
		if (strcmp(argv[i], "-j") == 0) {
			isJsonNext = true;
			continue;
		}
		
		if (isJsonNext) {
			args.parse(argv[1]);
		} else {
			std::filesystem::path path(argv[i]);
			
			if (fs::exists(path) && fs::is_regular_file(path))
				args.paths.push_back(path);
		}
	}

	// if (argc >= 2) {
		
	// 	// TODO check if file access or raw json
	// }
	
	MyApp app(args);
	app.setup();
	app.start();

	return 0;
}