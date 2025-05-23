#pragma once

struct IApp {

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;

    virtual void setLight(bool enabled) = 0;
    

    // Test
    virtual int getTest() = 0;
    virtual void setTest(int value) = 0;
    virtual void printTest() = 0;
};