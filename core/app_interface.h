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

template<typename T>
struct Instanciator {

	Instanciator() = default;

	// Instanciator (const Instanciator&) = delete;
	// Instanciator& operator= (const Instanciator&) = delete;

	std::unique_ptr<T> make(std::string type) {
		if (instanciators.count(type) > 0) {
			return instanciators[type]();
		}

		std::cerr 
			<< "Unable to make entity of type " 
			<< type 
			<< ", maybe you should register your custom entity class using `registerType` ?" 
			<< std::endl;

		return nullptr;
	}
	
	inline void registerType(std::string type, std::function<std::unique_ptr<T>()> instanciatorFunc) {
		instanciators[type] = instanciatorFunc;
	}

	std::vector<std::string> listAvailableTypes() {
		std::vector<std::string> v;
		for (auto &[k, r] : instanciators)
			v.push_back(k);

		return v;
	}

	private:
	// Instanciator, enable entity instanciation
	// Register new instanciator for custom entity
	std::map<std::string, std::function<std::unique_ptr<T>()>> instanciators;
};

struct IApp {

	virtual void quit() = 0;

	virtual Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) = 0;



	virtual void clearScene() = 0;

	virtual void showOpenModelDialog() = 0;
	virtual void showSaveModelDialog() = 0;

	virtual void setCullMode(int mode) = 0;
	virtual bool getCull() const = 0;
	virtual void setCull(bool enabled) = 0;

	virtual std::map<std::string, std::shared_ptr<Model>>& getModels() = 0;
	virtual std::shared_ptr<Model> addModel(std::string type, std::string name) = 0;
	virtual void removeModel(std::string name) = 0;
	virtual Model& getCurrentModel() = 0;
	virtual std::shared_ptr<Model> getHoveredModel() = 0;

	virtual Model& getModel(std::string name) = 0;
	virtual int countModels() = 0;
	virtual bool hasModel(std::string name) = 0;
	virtual bool hasModels() = 0;
	virtual void clearModels() = 0;

	virtual std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) = 0;
	virtual std::string getSelectedModel() = 0;
	virtual bool setSelectedModel(std::string name) = 0;
	virtual std::string loadModel(const std::string& filename, std::string name = "") = 0;

	virtual void addColormap(const std::string name, const std::string filename) = 0;
	virtual void removeColormap(const std::string name) = 0;
	virtual void clearColormaps() = 0;
	virtual Colormap getColormap(const std::string name) = 0;
	virtual Colormap getColormap(int idx) = 0;

	virtual Snapshot snapshot() = 0;
	virtual void loadSnapshot() = 0;
	virtual std::vector<Snapshot> listSnapshots() = 0;
	virtual void saveState(const std::string filename) = 0;
	virtual void loadState(const std::string filename) = 0;

	virtual std::map<std::string, std::shared_ptr<Camera>>& getCameras() = 0;
	virtual std::shared_ptr<Camera> addCamera(std::string type, std::string name) = 0;
	virtual void removeCamera(std::string name) = 0;
	virtual Camera& getCurrentCamera() = 0;
	virtual Camera& getCamera(std::string name) = 0;
	virtual int countCameras() = 0;
	virtual bool hasCamera(std::string name) = 0;
	virtual bool hasCameras() = 0;
	virtual void clearCameras() = 0;
	// TODO add findOrCreate

	virtual bool setSelectedCamera(std::string selected) = 0;
	virtual std::string getSelectedCamera() = 0;

	virtual std::map<std::string, std::shared_ptr<Renderer>> getRenderers() = 0;
	virtual std::shared_ptr<Renderer> addRenderer(std::string type, std::string name) = 0;
	virtual void removeRenderer(std::string name) = 0;
	virtual Renderer& getRenderer(std::string name) = 0;
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
	virtual bool isDebug() const = 0;

	virtual const Instanciator<Model>& getModelInstanciator() const = 0;
	virtual const Instanciator<Camera>& getCameraInstanciator() const = 0;
	virtual const Instanciator<Renderer>& getRendererInstanciator() const = 0;

};