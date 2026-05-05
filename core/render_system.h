#pragma once

#include "scene.h"
#include "shader_pass.h"

#include <map>
#include <string>
// #include <memory>

struct RenderSystem {

	// void render(Scene &scene) {
	// 	for (auto &[shaderPassName, shaderPass] : _shaderPasses) {
	// 		for (auto &[nodeId, node] : scene.getNodes()) {

	// 			auto &geometry = node->getGeometry();

	// 			for (auto &[materialName, material] : node->getMaterials()) {
					

	// 				if (node->isDirty()) {
						
	// 					// Dirty ? => get geometry data and push to gpu
	// 					material->fill(geometry);
	// 				}



	// 				GeometryBufferBase gb;

	// 				// Render node for shader pass
	// 				shaderPass->render(gb, *material, node->getWorldPosition());
	// 			}
	// 		}
	// 	}
	// }

	// std::map<std::string, std::unique_ptr<ShaderPass>> _shaderPasses; // all shader passes
	// std::map<std::string, GeometryBufferBase> geometryBuffers; // vao / vbo by node id

};