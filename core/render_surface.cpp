#include "render_surface.h"

#include <iostream>
#include "geometry.h"

void RenderSurface::setup() {
	// Framebuffer !
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Framebuffer texture
	glGenTextures(1, &texColor);
	glBindTexture(GL_TEXTURE_2D, texColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create depth texture
	glGenTextures(1, &depthAttachmentTexture);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create picking textures
	glGenTextures(1, &texCellID);
	glBindTexture(GL_TEXTURE_2D, texCellID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texFacetID);
	glBindTexture(GL_TEXTURE_2D, texFacetID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texVertexID);
	glBindTexture(GL_TEXTURE_2D, texVertexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &texMeshID);
	glBindTexture(GL_TEXTURE_2D, texMeshID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach color attachments to FBO buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texFacetID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texCellID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, texVertexID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, texMeshID, 0);
	// Attach depth attachments to FBO buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentTexture, 0);

	GLenum drawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, drawBuffers);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// Attach to FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete at setup." << std::endl;

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSurface::bind() {
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Enable all three color attachments at once
	GLenum drawBufs[5] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4
	};
	glDrawBuffers(5, drawBufs);
}

void RenderSurface::resize(int w, int h) {
	width = w;
	height = h;

	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Update color attachment texture
	glBindTexture(GL_TEXTURE_2D, texColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texVertexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texFacetID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texCellID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, texMeshID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Update depth/stencil renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	// Verify framebuffer completeness
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete at resize." << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_camera->updateScreenSize(width, height);
}

// void RenderSurface::clear() {
// 	// Clear attachments
// 	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.);


// 	GLfloat zero[5] = { 0.f, 0.f, 0.f, 0.f };
// 	// GLfloat bgColor[5] = { backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f };
// 	// GLfloat white[5] = { 1.f, 1.f, 1.f, 0.f };

// 	glClearBufferfv(GL_COLOR, 0, zero); // clear each float RT to 0
// 	glClearColor(1., 1., 1., 1.);

// 	glClearBufferfv(GL_COLOR, 1, zero); // clear each float RT to 0
// 	glClearBufferfv(GL_COLOR, 2, zero); // clear each float RT to 0
// 	glClearBufferfv(GL_COLOR, 3, zero); // clear each float RT to 0
// 	glClearBufferfv(GL_COLOR, 4, zero); // clear each float RT to 0
// }

void RenderSurface::clear() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Clear attachments
	GLfloat black[4] = { 0.f, 0.f, 0.f, 0.f };
	GLfloat bgColor[4] = { backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f };
	GLfloat depthClear = 1.0f;

	glClearBufferfv(GL_COLOR, 0, bgColor); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 1, black); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 2, black); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 3, black); // clear each float RT to 0
	glClearBufferfv(GL_COLOR, 4, black); // clear each float RT to 0

	glClearBufferfv(GL_DEPTH, 0, &depthClear);  // Clear depth to 1.0 (far plane)
}

void RenderSurface::render(Shader &screenShader, unsigned int quadVAO) {
	// // Go back to default framebuffer to draw the screen quad
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColor);

	glCullFace(GL_BACK);
	screenShader.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderSurface::clean() {
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &texColor);
	glDeleteTextures(1, &texCellID);
	glDeleteTextures(1, &texFacetID);
	glDeleteTextures(1, &texVertexID);
	glDeleteTextures(1, &texMeshID);
}


float RenderSurface::getDepth(double x, double y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    float depth;
    glReadPixels(x, height - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

void getNDC(int x, int y, int w, int h, float &ndcX, float &ndcY) {
	ndcX = (2.f*x) / w - 1.0f;
	ndcY = 1.0f - (2.f*y) / h;
}

void RenderSurface::unproject(int x, int y, float depth, vec3 &p) {
	// Screen coordinates to NDC
	float ndcX, ndcY;
	getNDC(x, y, width, height, ndcX, ndcY);

	// Clip space coordinates
	vec4 clipSpace{ndcX, ndcY, depth, 1.0f};

	// Unproject clip space to view space
	mat4x4 invProj = _camera->getProjectionMatrix().invert();
	vec4 viewSpace = invProj * clipSpace;

	// Unproject view space to world space
	mat4x4 invView = _camera->getViewMatrix().invert();
	vec4 worldSpace = invView * (viewSpace / viewSpace.data[3]);
	p = sl::vec4to3(worldSpace);
}

vec3 RenderSurface::pickPoint(double x, double y) {
	// Read depth value
    float depth = getDepth(x, y);
	depth = depth * 2.f - 1.f; // Convert to NDC range [-1, 1]

	vec3 p;
	unproject(x, y, depth, p);
	return p;
}

// long RenderSurface::pick(double x, double y) {	
// 	unsigned char pixel[4];
// 	glReadPixels(x, height- y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
// 	// Decode id from pixel
// 	return pixel[3] == 0 ? -1 :
// 		pixel[0] +
// 		pixel[1] * 256 +
// 		pixel[2] * 256 * 256;
// }

PickResult RenderSurface::pick2(double xPos, double yPos, int radius) {

	const int diameter = radius * 2 + 1;
	PickResult pickResult;

	// Allocate buffer for square that bounds our circle
	unsigned char* pixelData = new unsigned char[diameter * diameter * 4];

	// Read pixels in square that bounds our circle
	glReadPixels(
		xPos - radius,
		height - yPos - radius,
		diameter,
		diameter,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixelData
	);

	// Process each pixel in the bounding square
	for(int y = 0; y < diameter; ++y) {
		for(int x = 0; x < diameter; ++x) {
			// Calculate distance from center
			int dx = x - radius;
			int dy = y - radius;
			float distSq = dx * dx + dy * dy;

			// Only process pixels within circle
			if(distSq <= radius * radius) {
				int offset = (y * diameter + x) * 4;
				unsigned char r = pixelData[offset];
				unsigned char g = pixelData[offset + 1];
				unsigned char b = pixelData[offset + 2];
				unsigned char a = pixelData[offset + 3];
				
				long pickID = a == 0 ? -1 :
							r +
							g * 256 +
							b * 256 * 256;

				if (pickID != -1) {
					pickResult.set(x, y, pickID);
				}
			}
		}
	}

	delete[] pixelData;
	return pickResult;
}

// std::set<long> RenderSurface::pick(double xPos, double yPos, int radius) {

// 	const int diameter = radius * 2 + 1;
// 	std::set<long> pickIDs;

// 	// Allocate buffer for square that bounds our circle
// 	unsigned char* pixelData = new unsigned char[diameter * diameter * 4];

// 	// Read pixels in square that bounds our circle
// 	glReadPixels(
// 		xPos - radius,
// 		height - yPos - radius,
// 		diameter,
// 		diameter,
// 		GL_RGBA,
// 		GL_UNSIGNED_BYTE,
// 		pixelData
// 	);

// 	// Process each pixel in the bounding square
// 	for(int y = 0; y < diameter; ++y) {
// 		for(int x = 0; x < diameter; ++x) {
// 			// Calculate distance from center
// 			int dx = x - radius;
// 			int dy = y - radius;
// 			float distSq = dx * dx + dy * dy;

// 			// Only process pixels within circle
// 			if(distSq <= radius * radius) {
// 				int offset = (y * diameter + x) * 4;
// 				unsigned char r = pixelData[offset];
// 				unsigned char g = pixelData[offset + 1];
// 				unsigned char b = pixelData[offset + 2];
// 				unsigned char a = pixelData[offset + 3];
				
// 				long pickID = a == 0 ? -1 :
// 							r +
// 							g * 256 +
// 							b * 256 * 256;

// 				if (pickID != -1) {
// 					pickIDs.insert(pickID);
// 				}
// 			}
// 		}
// 	}

// 	delete[] pixelData;
// 	return pickIDs;
// }




PickResult RenderSurface::pickMeshes(double x, double y, int radius) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
	auto result = pick2(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	// Clean up results
	return result;
	// return id >= 0 && id < Geometry::getMaxIndex() ? id : -1;
}

PickResult RenderSurface::pickVertices(double x, double y, int radius) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	auto result = pick2(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return result;
	// Clean ids
	// std::vector<long> clean_ids;
	// std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
	// 	return id >= 0 && id < geometry.nverts();
	// });
	// return clean_ids;
}

PickResult RenderSurface::pickFacets(double x, double y, int radius) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	auto result = pick2(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return result;
	// Clean ids
	// std::vector<long> clean_ids;
	// std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
	// 	return id >= 0 && id < geometry.nverts();
	// });
	// return clean_ids;
}

PickState RenderSurface::getPickState(double x, double y, int radius) {
	auto pickMeshesResult = pickMeshes(x, y, radius);
	auto pickVerticesResult = pickVertices(x, y, radius);
	auto pickFacetsResult = pickFacets(x, y, radius);

	std::array<PickResult, PickElement::PICK_ELEMENT_COUNT> results;
	results[PickElement::PICK_MESH] = pickMeshesResult;
	results[PickElement::PICK_VERTEX] = pickVerticesResult;
	results[PickElement::PICK_FACET] = pickFacetsResult;

	return PickState(results);
}

// long RenderSurface::pickEdge(double x, double y) {
// 	// if (!st.cell.anyHovered() && !st.facet.anyHovered())
// 	// 	return -1;

// 	auto geometryOpt = scene.getHoveredMesh();

// 	if (!geometryOpt.has_value())
// 		return -1;

// 	auto &geometry = geometryOpt.value().get();

// 	auto p = pickPoint(x, y);

// 	int c = st.cell.getHovered();
// 	if (c < 0)
// 		c = st.facet.getHovered();

// 	return geometry.pickEdge(p, c);

// }

// std::vector<long> RenderSurface::pickFacets(double x, double y, int radius) {
// 	auto geometryOpt = scene.getHoveredMesh();

// 	if (!geometryOpt.has_value())
// 		return {};

// 	auto &geometry = geometryOpt.value().get();

// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, scene.getDefaultRenderSurface().fbo);
// 	glReadBuffer(GL_COLOR_ATTACHMENT1);
// 	auto ids = pick(x, y, radius);
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

// 	// Clean ids
// 	std::vector<long> clean_ids;
// 	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
// 		return id >= 0 && id < geometry.nfacets();
// 	});

// 	return clean_ids;
// }

// std::vector<long> RenderSurface::pickCells(double x, double y, int radius) {		
// 	auto geometryOpt = scene.getHoveredMesh();

// 	if (!geometryOpt.has_value())
// 		return {};

// 	auto &geometry = geometryOpt.value().get();

// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, scene.getDefaultRenderSurface().fbo);
// 	glReadBuffer(GL_COLOR_ATTACHMENT2);
// 	auto ids = pick(x, y, radius);
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

// 	// Clean ids
// 	std::vector<long> clean_ids;
// 	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
// 		return id >= 0 && id < geometry.ncells();
// 	});

// 	return clean_ids;
// }