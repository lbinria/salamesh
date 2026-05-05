#pragma once 
#include <string>
#include <vector>

struct GeometryLayout {
	
	enum VertexAttributeType {
		VERTEX_ATTRIBUTE_TYPE_INT,
		VERTEX_ATTRIBUTE_TYPE_UINT,
		VERTEX_ATTRIBUTE_TYPE_FLOAT,
		VERTEX_ATTRIBUTE_TYPE_VEC2,
		VERTEX_ATTRIBUTE_TYPE_VEC3
	};

	struct VertexAttribute {
		std::string name;
		VertexAttributeType type;
		int offset;
	};

	enum RenderPrimitive {
		RENDER_POINTS,
		RENDER_LINES,
		RENDER_TRIANGLES
	};

	size_t stride = 0;
	std::vector<VertexAttribute> vertexAttributes;
	std::vector<std::string> texBufferNames;
	RenderPrimitive renderPrimitive;
};




