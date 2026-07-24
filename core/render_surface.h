#pragma once
#include "camera.h"
#include "shader.h"

#include <memory>
#include <set>
#include "data/picking.h"

struct RenderSurface {
	


	RenderSurface(int w, int h) : width(w), height(h) {

		
	}

	unsigned int fbo;
	unsigned int rbo;
	
	unsigned int depthAttachmentTexture;
	unsigned int texColor;
	unsigned int texVertexID;
	unsigned int texHalfedgeID;
	unsigned int texFacetID;
	unsigned int texCellID;
	unsigned int texMeshID;

	int width, height;
	sl::algebra::vec3 backgroundColor;


	void setup();
	void bind();
	void clear();
	void render(Shader &screenShader, unsigned int quadVAO);
	void resize(int w, int h);
	void clean();

	// Add picking functions
	vec3 pickPoint(double x, double y);
	PickResult readBufferPixels(double xPos, double yPos, int radius, int element);
	PickResult readBufferPixels(double xPos, double yPos, int width, int height, int element);

	PickResult pick(PickElement element, double x, double y, int radius);
	PickResult pick(PickElement element, double x, double y, int width, int height);

	PickState getPickState(double x, double y, int radius);

	// std::vector<long> pickFacets(double x, double y, int radius) override;
	// std::vector<long> pickCells(double x, double y, int radius) override;
	float getDepth(double x, double y);
	void unproject(int x, int y, float depth, vec3 &p);

	void setCamera(std::shared_ptr<Camera> camera) {
		_camera = camera;
		_camera->updateScreenSize(width, height);
	}

	void setBackgroundColor(sl::algebra::vec3 color) {
		backgroundColor = color;
	}

	Camera& getCamera() { return *_camera; }

	private:
	std::shared_ptr<Camera> _camera;

	unsigned int pickPBO[2 * PickElement::PICK_ELEMENT_COUNT]; // double buffering per element (so 2 buffer per elments)
	int currentPBO[PickElement::PICK_ELEMENT_COUNT] = {0};
};