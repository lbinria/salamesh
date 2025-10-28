#pragma once 

#include "app.h"
#include "script.h"
#include "helpers/args_manager.h"

struct MyApp : App {

	MyApp(Args args) : App(args) {}

    void loadModel(const std::string& filename) override;
    int addModel(std::string name) override;
    void removeModel(int idx) override;
    bool removeModel(std::string name) override;
	std::shared_ptr<Model> getModelByName(std::string name) override;
	int getIndexOfModel(std::string name) override;

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

	std::unique_ptr<Camera> makeCamera(std::string type) override;

	// Move that in a layout component
	void TopModePanel(int &currentMode, const std::vector<std::pair<std::string, ImTextureID>>& icons, ImVec2 iconSize = ImVec2(28,28));

	private:

};