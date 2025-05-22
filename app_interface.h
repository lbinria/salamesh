#pragma once

struct IApp {
    virtual void reset_zoom() = 0;
    virtual void look_at_center() = 0;
};