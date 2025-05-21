#pragma once

#include <string>
#include <vector>

struct Tool {

	// virtual std::string get_name() = 0;

	Tool() {}
 	virtual ~Tool() = default;

	// /**
	//  * Check whether the tool is compatible with current mesh according to its metadata
	//  */
	// virtual bool is_compatible() = 0;

	virtual void init() = 0;
	virtual void setup() = 0;
	virtual void cleanup() = 0;

	/**
	 * Draw GUI
	 * Return true if selected tool has changed
	 */
	virtual void draw_gui() = 0;

	virtual void mouse_move(double x, double y) {}
	virtual void mouse_button(int button, int action, int mods) {}
	virtual void key_event(int key, int scancode, int action, int mods)	{}

	// TODO see if good idea to make callback on primitives
	// Because its higher level
	virtual void cell_hover(std::vector<int> ids) {}
	virtual void facet_hover(std::vector<int> ids) {}
	virtual void edge_hover(std::vector<int> ids) {}
	virtual void point_hover(std::vector<int> ids) {}

	virtual void cell_select(std::vector<int> ids) {}
	virtual void facet_select(std::vector<int> ids) {}
	virtual void edge_select(std::vector<int> ids) {}
	virtual void point_select(std::vector<int> ids) {}



	// /**
	//  * Call when user press escape key
	//  */
	// virtual void escape_callback() = 0;

	// virtual void validate_callback() = 0;


	// virtual void scroll_callback(double xoffset, double yoffset) = 0;
	// virtual void hover_callback(double x, double y, int source) = 0;

	/**
	 * Clear tool
	 */
	// virtual void clear() = 0;


};