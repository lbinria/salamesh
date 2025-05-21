#pragma once

#include <ultimaille/all.h>
#include "tool.h"

#include "model.h"
#include "states.h"

struct DebugTool : public Tool {

	// TODO add model_gfx for modifying view
	DebugTool(Model &model, InputState &st) : Tool(),  model(model), st(st) {}

	void init() override {};
	void setup() override {};
	void cleanup() override {};
	void draw_gui() override;

	private:

	Model &model;
	InputState &st;


};