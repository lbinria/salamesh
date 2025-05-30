#pragma once

#include "hex_mesh.h"
#include "arcball_camera.h"
// #include <memory>

struct IApp {

    enum Element {
        CORNERS = -1,
        POINTS = 0,
        EDGES = 1,
        FACETS = 2,
        CELLS = 3
    };


    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;

    virtual bool getLight() = 0;
    virtual void setLight(bool enabled) = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual void setCullMode(int mode) = 0;

    virtual ArcBallCamera& getCamera() = 0;
    virtual HexRenderer& getRenderer() = 0;

    virtual std::vector<std::string> getPickModeStrings() const = 0;

    virtual int getPickMode() = 0;
    virtual void setPickMode(Element mode) = 0;
};