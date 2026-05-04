#pragma once

#include "scene.h"
#include "render_surface.h"
#include "models/model.h"
#include "data/image.h"
#include "data/snapshot.h"
#include "navigation_path.h"

#include "input_states.h"
#include "data/element_type.h"
#include "cameras/camera.h"
#include "cameras/camera_collection.h"
#include "renderers/renderer_collection.h"
#include "models/model_collection.h"
#include <string>


struct IApp {

	virtual void quit() = 0;

	virtual Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) = 0;


	virtual void showOpenModelDialog() = 0;
	virtual void showSaveModelDialog() = 0;

	virtual void setCullMode(int mode) = 0;
	virtual bool getCull() const = 0;
	virtual void setCull(bool enabled) = 0;

	virtual void clearScene() = 0;


	virtual Scene& getScene() = 0;


	virtual Snapshot snapshot() = 0;
	virtual void loadSnapshot() = 0;
	virtual std::vector<Snapshot> listSnapshots() = 0;
	virtual void saveState(const std::string filename) = 0;
	virtual void loadState(const std::string filename) = 0;

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