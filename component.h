struct Component {

    // Lifecycle
    virtual void init() = 0;
    virtual bool draw_gui() = 0;

    // Events
    virtual void key_event(int key, int scancode, int action, int mods) = 0;
};