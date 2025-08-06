#include "app.h"
#include "script.h"
#include "helpers/args_manager.h"

struct MyApp : App {

	MyApp(Args args) : App(args) {}

    void loadModel(const std::string& filename) override;
	// TODO addModel / removeModel functions

	// Override lifecycle functions
	void init() override;
	void update(float dt) override;
	void draw_gui() override;

	// Override event functions
    void key_event(int key, int scancode, int action, int mods) override;
	void mouse_scroll(double xoffset, double yoffset) override;
	void mouse_button(int button, int action, int mods) override;
	void mouse_move(double x, double y) override;

    // States functions
    void save_state(const std::string filename);
    void load_state(const std::string filename);

	private:

};