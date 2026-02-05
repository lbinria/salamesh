#pragma once

#include "models/model.h"
#include "input_states.h"
#include "element.h"
#include "cameras/camera.h"
#include <string>

struct Colormap {
	std::string name;
	int width, height;
	unsigned int tex;
};

struct Image {
	unsigned int texId;
	int width, height, channels;
	unsigned int getTexId() { return texId; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	int getChannels() { return channels; }
};

struct Snapshot {
	std::string stateFilename;
	std::string thumbFilename;
	Image image;

	std::string getStateFilename() { return stateFilename; }
	std::string getThumbfilename() { return thumbFilename; }
	Image getImage() { return image; }
};

struct IApp {

	virtual void quit() = 0;

	virtual Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) = 0;

	virtual int addModel(std::string name, ModelType type) = 0;
	virtual void removeModel(int idx) = 0;
	virtual bool removeModel(std::string name) = 0;
	virtual bool loadModel(const std::string& filename) = 0;

	virtual void clearScene() = 0;

	virtual void showOpenModelDialog() = 0;
	virtual void showSaveModelDialog() = 0;

	virtual void setCullMode(int mode) = 0;
	virtual bool getCull() const = 0;
	virtual void setCull(bool enabled) = 0;

	
	virtual std::vector<std::shared_ptr<Model>>& getModels() = 0;
	virtual std::shared_ptr<Model> getModelByName(std::string name) = 0;
	virtual int getIndexOfModel(std::string name) = 0;
	virtual std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) = 0;
	virtual int countModels() = 0;
	virtual bool hasModels() = 0;
	virtual int getSelectedModel() = 0;
	virtual void setSelectedModel(int selected) = 0;
	virtual Model& getCurrentModel() = 0;
	virtual void addColormap(const std::string name, const std::string filename) = 0;
	virtual void removeColormap(const std::string name) = 0;
	virtual Colormap getColormap(const std::string name) = 0;
	virtual Colormap getColormap(int idx) = 0;

	virtual Snapshot snapshot() = 0;
	virtual void loadSnapshot() = 0;
	virtual std::vector<Snapshot> listSnapshots() = 0;
	virtual void saveState(const std::string filename) = 0;
	virtual void loadState(const std::string filename) = 0;

	virtual Camera& addCamera(std::string type, std::string name) = 0;
	virtual void removeCamera(std::string name) = 0;
	virtual Camera& getCamera() = 0;
	virtual std::vector<std::shared_ptr<Camera>>& getCameras() = 0;
	virtual void setSelectedCamera(int selected)= 0;
	virtual int getSelectedCamera() = 0;
	virtual int countCameras() = 0;

	virtual std::map<std::string, std::shared_ptr<IRenderer>> getRenderers() = 0;
	virtual IRenderer& addRenderer(std::string type, std::string name) = 0;
	virtual void removeRenderer(std::string name) = 0;
	virtual IRenderer& getRenderer(std::string name) = 0;
	virtual int countRenderers() = 0;
	virtual bool hasRenderer(std::string name) = 0;
	virtual bool hasRenderers() = 0;
	virtual void clearRenderers() = 0;

	virtual InputState& getInputState() = 0;

	// Window & rendersurface
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual double getAspectRatio() const = 0;

	virtual int getSurfaceWidth() const = 0;
	virtual int getSurfaceHeight() const = 0;

	virtual std::vector<Colormap> getColormaps() = 0;

	// Navigation
	virtual std::vector<std::string> getNavigationPath() = 0;
	virtual void setNavigationPath(std::vector<std::string> path) = 0;
	virtual std::string getNavigationPathString() = 0;
	virtual void addNavigationPath(std::string pathComponent) = 0;
	virtual void topNavigationPath() = 0;

	// Modules
	virtual void loadModule(fs::path modulePath) = 0;

	virtual bool isUIHovered() const = 0;

	// Registration
	virtual std::vector<std::string> listAvailableCameras() = 0;
	virtual std::vector<std::string> listAvailableRenderers() = 0;
};