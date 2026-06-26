#pragma once
#include "camera.h"

struct TrackBallCamera : public Camera {

    TrackBallCamera(std::string name) : Camera(name) {}

    vec4 getBounds() {
        float zoomFactor = _zoomFactor + 0.00001f; // Add eps to avoid screen size = 0 at 100%

        auto [min, max] = _box;
        auto wh = max - min;
        auto half = wh / 2.f;

        auto c = (min + max) * .5f;

        float bound = wh.x > wh.y ? half.x : half.y;

        double aspect = _screen.x / _screen.y;

        return {
            -bound * aspect * zoomFactor,
            bound * aspect * zoomFactor,
            -bound * zoomFactor,
            bound * zoomFactor
        };
    }

    void updateProjectionMatrix() override {
        auto b = getBounds();
        _proj = sl::ortho(b.data[0], b.data[1], b.data[2], b.data[3], _nearPlane, _farPlane);
    }

    void lookAtBox(std::tuple<vec3, vec3> box) override {
        _zoomFactor = 1.f;

        auto [min, max] = box;
        auto c = (min + max) * .5f;

        // Setup view matrix
        _pos = {c.x, c.y, c.z + (max - min).norm2()};
        _lookAt = c;

        _view = sl::lookAt(_pos, _lookAt, _up);

        _box = box;

        // Setup proj matrix
        updateProjectionMatrix();
    }

    vec3 mouseToSphere(vec2 p) {
        // Screen coords to NDC
        vec2 v{p.x / _screen.x * 2. - 1., -(p.y / _screen.y * 2. - 1.)};
        // v = -v / 1.96f; // Division make the sphere radius greater than 1
        // Division make the sphere radius greater than 1 therefore the border of the sphere is out of screen and this enable to not drag out of the sphere
        v = -v / 2.;

        // Compute magnitude of v (dist² to the center)
        double mag = v * v;
        vec3 p3{v.x, v.y, 0.};

        if (mag > 1.0) {
            p3 = p3.normalized();
        } else {
            p3 = {p3.x, p3.y, -sqrt(1.0 - mag)};
        }

        return p3;
    }

    void move(vec2 oldPos, vec2 newPos) override {
        if (_isLocked)
            return;

        // Compute 3D pos of 2D point on sphere
        vec3 v0 = mouseToSphere(oldPos);
        vec3 v1 = mouseToSphere(newPos);
        // Compute axis of rotation from 3D points
        vec3 ax = cross(v0, v1);

        // Check length to avoid normalize issues (division by 0 can occurs)
        if (ax.norm2() <= 0.00000001)
            return;

        // Compute angle between the two points on sphere
        double angle = acos(std::clamp(v0 * v1, -1., 1.)) * 1.5 /* speed */;

        // Create quaternion from axis, angle for rotation
        auto q = sl::angleAxis(angle, ax.normalized());
        
        // Translate view to origin for pivot
        auto [min, max] = _box;
        auto c = (min + max) * .5f;

        _view = sl::translate(_view, c);

        // Rotate view
        _view[0] = sl::rotate(_view[0], q);
        _view[1] = sl::rotate(_view[1], q);
        _view[2] = sl::rotate(_view[2], q);

        // Translate view back
        _view = sl::translate(_view, -c);


        // Just update to know where is the camera
        // vec4 position(_pos.x, _pos.y, _pos.z, 1);
        // vec4 pivot(_lookAt.x, _lookAt.y, _lookAt.z, 1);
        // position = (q * (position - pivot)) + pivot;
        // _pos = position;

        auto cameraMatrix = _view.invert();
        auto camPos = cameraMatrix[3];
        
        // std::cout << "eye: " << _pos.x << ", " << _pos.y << ", " << _pos.z <<  std::endl;
        // std::cout << "cam pos: " << camPos.x << ", " << camPos.y << ", " << camPos.z << ", " << camPos.w << std::endl;

        _pos = sl::vec4to3(camPos);

    }

    void movePan(vec2 delta) {
        if (_isLocked)
            return;

        // Compute view rect size and divide by screen rect size 
        // to get how many world unit per pixel
        auto b = getBounds();
        vec2 viewDims{b.data[1] - b.data[0], b.data[3] - b.data[2]};
        vec2 worldUnitPerPixel = sl::div(viewDims, _screen);

        // Get offset in world coordinates
        vec2 offset = sl::mul(worldUnitPerPixel, delta);

        vec3 right = getRightVector();
        vec3 up = getUpVector();


        _view = sl::translate(_view, right * offset.x + up * -offset.y);

        _pos = sl::vec4to3(_view.invert()[3]);
    }

    void moveRight(double speed) override {
        movePan({speed * 500., 0});
    }

    void moveForward(double speed) override {
        zoom(speed * 10.);
    }

    void moveUp(double speed) override {
        movePan({0, speed * 500.});
    }

    void zoom(float delta) {
        // fine-tuned using desmos graph with formula: (1/\ (1+\exp(-(x-c)/w)))*m*2
        // goal is to have greater factor when around _zoomFactor >= 1
        // Change m (max_value) for adjusting speed, but this influences c, w (center, width)
        // Maybe we can found formula to adjust c, w automatically given m
        // or just multiplying delta will be sufficient...
        float factor = sigmoid(_zoomFactor, 0.8f, 0.2f, 0.08f /* factor (max slope of sigmoid) */);
        
        _zoomFactor = std::clamp(_zoomFactor + delta * factor, 0.f, 10.f); 
        updateProjectionMatrix();
    }

    void resetZoom() {
        _zoomFactor = 1.f;
        updateProjectionMatrix();
    }

    void doSaveState(json &j) {
        auto [min, max] = _box;
        j["box"] = json::array({min.x, min.y, min.z, max.x, max.y, max.z});
    }

    void doLoadState(json &j) {
        auto &jBox = j["box"];

        _box = std::make_tuple(
            vec3{jBox[0], jBox[1], jBox[2]},
            vec3{jBox[3], jBox[4], jBox[5]}
        );
    }

    std::string getType() override { return "TrackBallCamera"; }

    private:

    std::tuple<vec3, vec3> _box; // Targeted bounding box
};