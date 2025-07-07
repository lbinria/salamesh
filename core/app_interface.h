#pragma once

#include "renderer.h"
#include "states.h"
#include "element.h"
#include "arcball_camera.h"

struct IApp {

    virtual void screenshot(const std::string& filename) = 0;
    virtual void load_model(const std::string& filename) = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    // TODO move this to camera
    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;

    // TODO maybe remove followings
    virtual bool getLight() = 0;
    virtual void setLight(bool enabled) = 0;
    virtual void setClipping(bool enabled) = 0;

    virtual void setCullMode(int mode) = 0;

    virtual ArcBallCamera& getCamera() = 0;
    
    virtual std::vector<std::unique_ptr<Renderer>>& getRenderers() = 0;
    virtual int countRenderers() = 0;
    virtual int getSelectedRenderer() = 0;
    virtual void setSelectedRenderer(int selected) = 0;
    virtual Renderer& getCurrentRenderer() = 0;

    virtual InputState& getInputState() = 0;

    virtual std::vector<std::string> getPickModeStrings() const = 0;

    // TODO maybe useless when picking functions below will be implemented
    virtual int getPickMode() = 0;
    virtual void setPickMode(Element mode) = 0;


    // virtual glm::vec3 pick_point(long x, long y) = 0;
    // virtual long pick_vertex() = 0;
    // TODO see if necessary here ! maybe pick_edge() is sufficient
    virtual long pick_edge(Volume &m, glm::vec3 p0, int c) = 0;
    virtual long pick_edge() = 0;
    virtual long pick_facet() = 0;
    virtual long pick_cell() = 0;

    virtual std::vector<unsigned int> getColorMaps2D() = 0;


};