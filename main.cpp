#include "my_app.h"
#include "helpers/args_manager.h"

int main(int argc, char** argv) {
	
	Args args;
	if (argc >= 2) {
		// TODO check if file access or raw json
		args.parse(argv[1]);
	}
	
	MyApp app(args);
	app.setup();
	app.start();

	return 0;
}