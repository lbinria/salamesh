#pragma once

#include "scene.h"
#include "render_surface.h"
#include "image.h"
#include "snapshot.h"
#include "navigation_path.h"

#include "io/input_states.h"
#include "element_type.h"
#include "camera.h"
#include "camera_collection.h"
#include <string>

#include "transformer.h"

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

	virtual TransformRegistry& getTransformRegistry() = 0;

};