#pragma once

#include "models/model.h"
#include "input_states.h"
#include "element.h"
#include "camera.h"
#include <string>

struct Colormap {
    std::string name;
    int width, height;
    unsigned int tex;
};

struct IApp {

    virtual void screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) = 0;

    virtual int addModel(std::string name, ModelType type) = 0;
    virtual void removeModel(int idx) = 0;
    virtual bool removeModel(std::string name) = 0;
    virtual bool loadModel(const std::string& filename) = 0;

    virtual void clearScene() = 0;

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

    virtual void snapshot() = 0;
    virtual void loadSnapshot() = 0;
    virtual std::vector<std::vector<std::string>> listSnapshots() = 0;
    virtual void saveState(const std::string filename) = 0;
    virtual void loadState(const std::string filename) = 0;

    virtual Camera& getCamera() = 0;
    virtual std::vector<std::shared_ptr<Camera>>& getCameras() = 0;
    virtual int countCameras() = 0;
    virtual void setSelectedCamera(int selected)= 0;
    virtual int getSelectedCamera() = 0;



    virtual InputState& getInputState() = 0;

    virtual int getScreenWidth() const = 0;
    virtual int getScreenHeight() const = 0;
    virtual double getScreenAspectRatio() const = 0;


    virtual std::vector<Colormap> getColormaps() = 0;

    virtual std::vector<std::string> getNavigationPath() = 0;
    virtual void setNavigationPath(std::vector<std::string> path) = 0;
    virtual std::string getNavigationPathString() = 0;
    virtual void addNavigationPath(std::string pathComponent) = 0;

};