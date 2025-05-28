struct Component {

    // Lifecycle
    virtual void init() = 0;
    virtual bool draw_gui() = 0;

    // Events
	virtual void mouse_move(double x, double y) = 0;
	virtual void mouse_button(int button, int action, int mods) = 0;
    virtual void mouse_scroll(double xoffset, double yoffset) = 0;
    virtual void key_event(int key, int scancode, int action, int mods) = 0;
};