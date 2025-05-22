#pragma once

struct IApp {
    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;
    virtual int getTest() = 0;
    virtual void setTest(int value) = 0;
    virtual void printTest() = 0;
};