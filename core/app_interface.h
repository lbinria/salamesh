#pragma once

#include "scene_interface.h"
#include "render_surface.h"
#include "models/model.h"
#include "input_states.h"
#include "element.h"
#include "cameras/camera.h"
#include "cameras/camera_collection.h"
#include "renderers/renderer_collection.h"
#include "models/model_collection.h"
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

struct NavigationPath {

	NavigationPath(std::vector<std::string> path = {}) : 
		path(path) {}

	void set(std::vector<std::string> newPath) {
		path = newPath;
	}

	void set(std::string newPath) {
		auto pathComponents = split(newPath, '/');
		set(pathComponents);
	}

	std::vector<std::string> get() const {
		return path;
	}

	std::string str() const {
		if (path.empty()) return {};

		std::ostringstream oss;
		oss << path[0];
		for (size_t i = 1; i < path.size(); ++i) 
			oss << '/' << path[i];

		return oss.str();
	}

	operator std::string() const {
		return str();
	}

	bool operator ==(const NavigationPath &other) const {
		return this->path == other.path;
	}

    // Compare with std::string
    bool operator==(const std::string& str) const {
        return this->str() == str;
    }
    
    bool operator!=(const NavigationPath& other) const {
        return !(*this == other);
    }
    
    bool operator!=(const std::string& str) const {
        return !(this->str() == str);
    }

	void push(std::string pathComponent) {
		auto oldPath = path;
		path.push_back(pathComponent);
	}

	void pop() {
		if (path.size() <= 0)
			return;

		auto oldPath = path;
		path.erase(path.end() - 1);
	}

	bool startsWith(std::string strPath) const {
		auto pathComponents = split(strPath, '/');
		return startsWith(pathComponents);
	}

	bool startsWith(std::vector<std::string> head) const noexcept {
		if (head.size() > path.size())
			return false;

		for (int i = 0; i < head.size(); ++i) {
			if (path[i] != head[i])
				return false;
		}

		return true;
	}

	private:
	std::vector<std::string> path;

	std::vector<std::string> split(std::string s, char delim) const {
		std::stringstream ss(s);
		std::string segment;
		std::vector<std::string> segments;

		while(std::getline(ss, segment, delim))
			segments.push_back(segment);

		return segments;
	}

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


	virtual void showOpenModelDialog() = 0;
	virtual void showSaveModelDialog() = 0;

	virtual void setCullMode(int mode) = 0;
	virtual bool getCull() const = 0;
	virtual void setCull(bool enabled) = 0;

	virtual void clearScene() = 0;

	virtual void computeFarPlane() = 0;

	virtual IScene& getScene() = 0;



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



	virtual RenderSurface &getRenderSurface() = 0;

	virtual InputState& getInputState() = 0;

	// Window & rendersurface
	virtual int getWindowWidth() const = 0;
	virtual int getWindowHeight() const = 0;
	virtual double getAspectRatio() const = 0;

	virtual glm::vec3 pickPoint(double x, double y) = 0;
	virtual long pickEdge(double x, double y) = 0;
	virtual long pick_mesh(double x, double y) = 0;
	virtual std::vector<long> pick_vertices(double x, double y, int radius) = 0;
	virtual std::vector<long> pick_facets(double x, double y, int radius) = 0;
	virtual std::vector<long> pick_cells(double x, double y, int radius) = 0;

	virtual std::vector<Colormap> getColormaps() = 0;

	// Navigation
	virtual NavigationPath getNavigationPath() = 0;
	virtual void setNavigationPath(std::vector<std::string> path) = 0;
	virtual void setNavigationPath(std::string strPath) = 0;
	virtual void addNavigationPath(std::string pathComponent) = 0;
	virtual void topNavigationPath() = 0;

	// Modules
	virtual void loadModule(fs::path modulePath) = 0;

	virtual bool isUIHovered() const = 0;
	virtual bool isDebug() const = 0;

};