#pragma once
#include "renderer.h"

struct MeshRenderer : public Renderer {
	
	MeshRenderer(std::string name, Shader shader) : 
		Renderer(name, std::move(shader))
		{}


	// TODO to remove
	virtual void doLoadState(json &j) override {}
	virtual void doSaveState(json &j) const override {}

	protected:
		
};