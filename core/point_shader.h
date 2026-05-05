#pragma once
#include "shader_pass.h"

#include <ultimaille/all.h>
#include "data/material.h"
#include "shader.h"

#include "scene_node.h"
#include "data/geometry.h"

#include "helpers.h"

#include "utils/opengl_helper.h"


struct PointShaderPass : ShaderPass {

	struct PointMaterialParams : MaterialParams {

		struct LightMaterialParams : MaterialParams {
			bool enabled = true;

			void apply(Shader &shader) const override {
				shader.setBool("light_enabled", enabled);
			}
		};

		struct PointStyleMaterialParams : MaterialParams {
			float size = 4.f;
			glm::vec3 color{1., 1., 1.};

			void apply(Shader &shader) const override {
				shader.setFloat("pointSize", size);
				shader.setFloat3("pointColor", color);
			}
		};

		PointStyleMaterialParams style;
		LightMaterialParams light;

		void apply(Shader &shader) const override {
			style.apply(shader);
			light.apply(shader);
		}
	};

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
	};

	struct PointGeometryBuffer : GeometryBuffer {
		std::vector<Vertex> vertices;

		PointGeometryBuffer() {

		}
	};

	std::shared_ptr<Material> createMaterial() override {
		
		PointMaterial material;

		glBindVertexArray(material.vao());
		glBindBuffer(GL_ARRAY_BUFFER, material.vbo());
		
		// Init layout
		auto layout = getLayout();

		// Create VBO layout
		for (auto &attr : layout.vertexAttributes) {
			switch (attr.type) {
				case GeometryLayout::VertexAttributeType::VERTEX_ATTRIBUTE_TYPE_FLOAT: {
					sl::createVBOFloat(_shader.id, attr.name.c_str(), layout.stride, (void*)attr.offset);
					break;
				} case GeometryLayout::VertexAttributeType::VERTEX_ATTRIBUTE_TYPE_INT: {
					sl::createVBOInteger(_shader.id, attr.name.c_str(), layout.stride, (void*)attr.offset);
					break;
				} case GeometryLayout::VertexAttributeType::VERTEX_ATTRIBUTE_TYPE_VEC2: {
					sl::createVBOVec2(_shader.id, attr.name.c_str(), layout.stride, (void*)attr.offset);
					break;
				} case GeometryLayout::VertexAttributeType::VERTEX_ATTRIBUTE_TYPE_VEC3: {
					sl::createVBOVec3(_shader.id, attr.name.c_str(), layout.stride, (void*)attr.offset);
					break;
				}
			}
		}

		// Create TBOs buffers
		for (auto texBufferName : layout.texBufferNames) {
			unsigned int buf, tex;
			sl::createTBO(buf, tex);
			material.addTBO(texBufferName, {buf, tex});
		}

		return std::make_shared<PointMaterial>(material);
	}


	struct PointMaterial : Material {

		PointMaterial(const PointMaterial&) = delete;
		PointMaterial& operator=(const PointMaterial&) = delete;

		PointMaterial() {
			// Init params
			_params = std::make_shared<PointMaterialParams>();
		}

		std::string getType() const override {
			return "PointMaterial";
		}

		void fill(const Geometry &geometry) override {
			auto surfaceGeometry = dynamic_cast<const SurfaceGeometry*>(&geometry);
			
			if (surfaceGeometry) {
				auto &ps = surfaceGeometry->getSurface().points;
				PointShaderPass::PointGeometryBuffer pointGeometryBuffer;
				pointGeometryBuffer.vertices.resize(ps.size());
				

				for (int i = 0; i < ps.size(); ++i) {
					auto &v = ps[i];

					pointGeometryBuffer.vertices[i] = { 
						.vertexIndex = i,
						.position = glm::vec3(v.x, v.y, v.z),
						.size = 1.f
					};
				}

				pointGeometryBuffer.nverts = pointGeometryBuffer.vertices.size();

				pointGeometryBuffer.vboBuffer = {
					.data = pointGeometryBuffer.vertices.data(),
					.size = pointGeometryBuffer.vertices.size() * sizeof(Vertex)
				};

				_geometryBuffer = std::make_unique<PointGeometryBuffer>(pointGeometryBuffer);
			}
		}

	};

	PointShaderPass() : 
		ShaderPass(std::move(Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag")))) {
	}

	bool isMaterialSupported(std::shared_ptr<Material> material) const override {
		return material->getType() == "PointMaterial";
	}

	GeometryLayout getLayout() override {

		return {
			.stride = sizeof(Vertex),
			.vertexAttributes = {
				{"vertexIndex", GeometryLayout::VERTEX_ATTRIBUTE_TYPE_INT, (int)offsetof(Vertex, vertexIndex) },
				{"aPos", GeometryLayout::VERTEX_ATTRIBUTE_TYPE_VEC3, (int)offsetof(Vertex, position) },
				{"sizeScale", GeometryLayout::VERTEX_ATTRIBUTE_TYPE_FLOAT, (int)offsetof(Vertex, size) }
			},
			.texBufferNames = {"highlight", "filter", "colormap_0", "colormap_1", "colormap_2"},
			.renderPrimitive = GeometryLayout::RENDER_POINTS
		};
	}

	void render(const Material &material, glm::vec3 position) override {

		// Try to cast to the required type
		auto pointMaterial = dynamic_cast<const PointMaterial*>(&material);

		// If GeometryBuffer isn't PointGeometryBuffer => 
		// it is incompatible with this shader pass and it cannot be rendered here
		if (!pointMaterial) {
			return;
		}

		// Apply material to shader
		_shader.use();
		auto &params = material.getParams();
		params.apply(_shader);
		// ...

		glm::mat4 model = glm::mat4(1.0f);
		// model = glm::translate(model, node.getWorldPosition());
		model = glm::translate(model, position);
		_shader.setMat4("model", model);

		// Bind vertex array of pointBuffer for rendering
		glBindVertexArray(material.vao());

		// glActiveTexture(GL_TEXTURE0 + 0);
		// glBindTexture(GL_TEXTURE_2D, texColormap0);

		// glActiveTexture(GL_TEXTURE0 + 1);
		// glBindTexture(GL_TEXTURE_2D, texColormap1);

		// glActiveTexture(GL_TEXTURE0 + 2);
		// glBindTexture(GL_TEXTURE_2D, texColormap2);
		

		glDrawArrays(GL_POINTS, 0, material.getGeometryBuffer().nverts);
	}

	private:


};