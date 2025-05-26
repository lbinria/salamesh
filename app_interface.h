#pragma once

#include "hex_mesh.h"
#include "arcball_camera.h"
// #include <memory>

struct IApp {

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;

    virtual void setLight(bool enabled) = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual void setCullMode(int mode) = 0;

    virtual ArcBallCamera& getCamera() = 0;
    virtual HexRenderer& getRenderer() = 0;

    // Test
    virtual int getTest() = 0;
    virtual void setTest(int value) = 0;
    virtual void printTest() = 0;
};