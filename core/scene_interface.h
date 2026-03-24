#pragma once
#include "models/model_collection.h"
#include "cameras/camera_collection.h"

struct IScene {
	virtual void init() = 0;
	virtual void clean() = 0;
	virtual void clear() = 0;


	virtual std::shared_ptr<Model> loadModel(const std::string& filename, std::string name = "") = 0;
	virtual ModelCollection& getModels() = 0;
	virtual Model& getCurrentModel() = 0;
	virtual std::shared_ptr<Model> getHoveredModel() = 0;
	virtual std::string getSelectedModel() = 0;
	virtual bool setSelectedModel(std::string name) = 0;
	virtual void focus(std::string modelName) = 0;

	virtual CameraCollection& getCameras() = 0;
	// TODO add findOrCreate
	virtual Camera& getCurrentCamera() = 0;

	virtual bool setSelectedCamera(std::string selected) = 0;
	virtual std::string getSelectedCamera() = 0;
};