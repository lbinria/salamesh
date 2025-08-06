#include "app.h"
#include "script.h"
#include "helpers/args_manager.h"

struct MyApp : App {

	MyApp(Args args) : App(args) {}

	// Override lifecycle functions
	void init() override;
	void update(float dt) override;
	void draw_gui() override;

	// Override event functions
    void key_event(int key, int scancode, int action, int mods) override;
	void mouse_scroll(double xoffset, double yoffset) override;
	void mouse_button(int button, int action, int mods) override;
	void mouse_move(double x, double y) override;

	private:

};