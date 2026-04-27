#pragma once
#include "models/model_collection.h"
#include "cameras/camera_collection.h"
#include "renderers/renderer_collection.h"

#include "scene_view_interface.h"

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

	virtual RendererCollection& getRenderers() = 0;

	virtual ISceneView& getMainView() = 0;
	virtual ISceneView& getCurrentView() = 0;
	virtual std::map<std::string, std::shared_ptr<ISceneView>>& getViews() = 0;
	virtual ISceneView& addView(const std::string name) = 0;
	virtual std::string getSelectedView() = 0;
	virtual bool setSelectedView(const std::string name) = 0;

};