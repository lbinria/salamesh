#pragma once
#include "../core/camera.h"

struct TrackBallCamera : public Camera {

    using Camera::Camera;

    glm::vec4 getBounds() {
        float zoomFactor = _zoomFactor + 0.00001f; // Add eps to avoid screen size = 0 at 100%

        auto [min, max] = _box;
        auto wh = max - min;

        if (wh.x > wh.y) {
            float aspect = _screen.y / _screen.x;
            
            return {
                min.x * zoomFactor, 
                max.x * zoomFactor, 
                min.x * aspect * zoomFactor, 
                max.x * aspect * zoomFactor
            };

        } else {
            float aspect = _screen.x / _screen.y;
            
            return {
                min.y * aspect * zoomFactor, 
                max.y * aspect * zoomFactor, 
                min.y * zoomFactor, 
                max.y * zoomFactor
            };

        }
    }

    void updateProjectionMatrix() override {
        auto b = getBounds();
        m_projectionMatrix = glm::ortho(b.x, b.y, b.z, b.w, nearPlane, farPlane);
    }

    void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) override {
        _box = box;
        auto [min, max] = box;
        auto c = (max + min) * .5f;

        _zoomFactor = 1.f;
        m_eye = {c.x, c.y, c.z + glm::length(max - min) * 2.f};
        m_lookAt = c;

        updateViewMatrix();
        updateProjectionMatrix();
    }

    glm::vec3 mouseToSphere(glm::vec2 p) {
        // Screen coords to NDC
        glm::vec2 v{p.x / _screen.x * 2.f - 1.f, -(p.y / _screen.y * 2.f - 1.f)};
        // v = -v / 1.96f; // Division make the sphere radius greater than 1
        // Division make the sphere radius greater than 1 therefore the border of the sphere is out of screen and this enable to not drag out of the sphere
        v = -v / 2.f;

        // Compute magnitude of v (dist² to the center)
        float mag = glm::dot(v, v);
        glm::vec3 p3{v.x, v.y, 0.};

        if (mag > 1.0) {
            p3 = glm::normalize(p3);
        } else {
            p3 = {p3.x, p3.y, -sqrt(1.0 - mag)};
        }

        return p3;
    }

    void move(glm::vec2 oldPos, glm::vec2 newPos) override {
        if (m_lock)
            return;

        // Compute 3D pos of 2D point on sphere
        glm::vec3 v0 = mouseToSphere(oldPos);
        glm::vec3 v1 = mouseToSphere(newPos);
        // Compute axis of rotation from 3D points
        glm::vec3 ax = glm::cross(v0, v1);

        // Check length to avoid normalize issues (division by 0 can occurs)
        if (glm::length(ax) <= 0.00001f)
            return;

        // Compute angle between the two points on sphere
        float angle = acos(glm::clamp(glm::dot(v0, v1), -1.f, 1.f));

        // Create quaternion from axis, angle for rotation
        auto q = glm::angleAxis(angle, glm::normalize(ax));
        // Rotate view
        m_viewMatrix[0] = q * m_viewMatrix[0];
        m_viewMatrix[1] = q * m_viewMatrix[1];
        m_viewMatrix[2] = q * m_viewMatrix[2];

        // Just update to know where is the camera
        glm::vec4 position(m_eye.x, m_eye.y, m_eye.z, 1);
        glm::vec4 pivot(m_lookAt.x, m_lookAt.y, m_lookAt.z, 1);
        position = (q * (position - pivot)) + pivot;
        m_eye = position;


        // // Update translation T = -R^T*eye
        // m_viewMatrix[3] = glm::vec4(-glm::dot(glm::vec3(m_viewMatrix[0]), m_eye), -glm::dot(glm::vec3(m_viewMatrix[1]), m_eye), -glm::dot(glm::vec3(m_viewMatrix[2]), m_eye), 1.0f);


        auto cameraMatrix = glm::inverse(m_viewMatrix);
        auto camPos = cameraMatrix[3];
        // m_eye = camPos;
        std::cout << "eye: " << m_eye.x << ", " << m_eye.y << ", " << m_eye.z <<  std::endl;
        std::cout << "cam pos: " << camPos.x << ", " << camPos.y << ", " << camPos.z << ", " << camPos.w << std::endl;

    }

    void movePan(glm::vec2 delta) {
        if (m_lock)
            return;

        // Compute view rect size and divide by screen rect size 
        // to get how many world unit per pixel
        auto b = getBounds();
        glm::vec2 viewDims{b.y - b.x, b.w - b.z};
        glm::vec2 worldUnitPerPixel = viewDims / _screen;

        // Get offset in world coordinates
        glm::vec2 offset = worldUnitPerPixel * delta;

        glm::vec3 right = getRightVector();
        glm::vec3 up = getUpVector();

        glm::mat4 T(1.f);
        T = glm::translate(T, right * offset.x + up * -offset.y);
        m_viewMatrix = m_viewMatrix * T;
    }

    void moveRight(float speed) override {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed * 2.f;
        updateViewMatrix();
    }

    void moveForward(float speed) override {}
    void moveUp(float speed) override {}

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
            glm::vec3{jBox[0], jBox[1], jBox[2]},
            glm::vec3{jBox[3], jBox[4], jBox[5]}
        );

        updateProjectionMatrix();
    }

    std::string getType() override { return "TrackBallCamera"; }

    private:

    std::tuple<glm::vec3, glm::vec3> _box; // Targeted bounding box
};