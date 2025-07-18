#pragma once

#include "model.h"
#include "states.h"
#include "element.h"
#include "camera.h"

struct IApp {

    virtual void screenshot(const std::string& filename) = 0;
    virtual void load_model(const std::string& filename) = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    virtual void setCullMode(int mode) = 0;

    virtual Camera& getCamera() = 0;
    
    virtual std::vector<std::shared_ptr<Model>>& getModels() = 0;
    virtual int countModels() = 0;
    virtual int getSelectedModel() = 0;
    virtual void setSelectedModel(int selected) = 0;
    virtual Model& getCurrentModel() = 0;



    virtual std::vector<std::unique_ptr<Camera>>& getCameras() = 0;
    virtual int countCameras() = 0;
    virtual void setSelectedCamera(int selected)= 0;
    virtual int getSelectedCamera() = 0;



    virtual InputState& getInputState() = 0;

    virtual std::vector<std::string> getPickModeStrings() const = 0;

    // TODO maybe useless when picking functions below will be implemented
    virtual int getPickMode() = 0;
    virtual void setPickMode(Element mode) = 0;


    // virtual glm::vec3 pick_point(long x, long y) = 0;
    // virtual long pick_vertex() = 0;
    virtual long pick_edge(double x, double y) = 0;
    virtual long pick_facet(double x, double y) = 0;
    virtual long pick_cell(double x, double y) = 0;

    virtual std::vector<unsigned int> getColorMaps2D() = 0;


};