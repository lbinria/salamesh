/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

const float FAR_PLANE = 100.f;
const float NEAR_PLANE = 0.1f;

struct ArcBallCamera {

    ArcBallCamera() = default;

    ArcBallCamera(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::vec3 fovAndScreen) : 
        m_eye(std::move(eye)),
        m_target_eye(m_eye),
        m_lookAt(std::move(lookAt)),
        m_upVector(std::move(up)),
        m_projectionMatrix(glm::perspective(glm::radians(fovAndScreen.x), fovAndScreen.y / fovAndScreen.z, NEAR_PLANE, FAR_PLANE)),
        m_fovAndScreen(std::move(fovAndScreen))
    {
        UpdateViewMatrix();
    }

    void SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up, glm::mat4x4 projection)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookat);
        m_upVector = std::move(up);
        m_projectionMatrix = std::move(projection);
        UpdateViewMatrix();
    }

    void SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up, glm::vec3 fovAndScreen)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookat);
        m_upVector = std::move(up);
        m_projectionMatrix = glm::perspective(glm::radians(fovAndScreen.x), fovAndScreen.y / fovAndScreen.z, NEAR_PLANE, FAR_PLANE);
        UpdateViewMatrix();
    }

    void LookAt(glm::vec3 lookAt) {
        SetCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
    }

    void UpdateViewMatrix()
    {
        // Generate view matrix using the eye, lookAt and up vector
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    void Move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) {
        if (m_lock)
            return;

        // Compute delta angles from viewport dimensions
        glm::vec2 delta((2.f * M_PI) / viewportDims.x, M_PI / viewportDims.y);
        // Compute actual angles move
        float angleX = (lastMousePos.x - mousePos.x) * delta.x;
        float angleY = (lastMousePos.y - mousePos.y) * delta.y;


        // 
        float cosAngle = glm::dot(this->GetViewDir(), m_upVector);
        if (cosAngle * glm::sign(angleY) > 0.99f)
            angleY = 0;

        // std::cout << "angleX: " << angleX << ", angleY: "  << angleY << std::endl;

        glm::vec4 position(m_eye.x, m_eye.y, m_eye.z, 1);
        glm::vec4 pivot(m_lookAt.x, m_lookAt.y, m_lookAt.z, 1);

        // Rotate camera around pivot point
        glm::mat4 rx(1.f);
        rx = glm::rotate(rx, angleX, m_upVector);
        position = (rx * (position - pivot)) + pivot;

        glm::mat4 ry(1.f);
        ry = glm::rotate(ry, angleY, GetRightVector());
        position = (ry * (position - pivot)) + pivot;

        // Update camera
        SetCameraView(position, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void MovePlane(glm::vec2 deltaPos) {
        if (m_lock)
            return;

        glm::vec3 lookAt(m_lookAt.x, m_lookAt.y, m_lookAt.z);
        lookAt -= GetRightVector() * deltaPos.x * .005f;
        lookAt += GetUpVector() * deltaPos.y * .005f;
        // std::cout << "right vector: " << GetRightVector().x << ", " << GetRightVector().y << ", " << GetRightVector().z << std::endl;
        // std::cout << "deltaPos: " << deltaPos.x << ", " << deltaPos.y << std::endl;
        // position += m_upVector * deltaPos.y;
        SetCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
    }

    // void SmoothMove(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) {
    //     // Compute delta angles from viewport dimensions
    //     glm::vec2 delta((2.f * M_PI) / viewportDims.x, M_PI / viewportDims.y);
    //     // Compute actual angles move
    //     float angleX = (lastMousePos.x - mousePos.x) * delta.x;
    //     float angleY = (lastMousePos.y - mousePos.y) * delta.y;


    //     // 
    //     float cosAngle = glm::dot(this->GetViewDir(), m_upVector);
    //     if (cosAngle * glm::sign(angleY) > 0.99f)
    //         angleY = 0;

    //     // std::cout << "angleX: " << angleX << ", angleY: "  << angleY << std::endl;

    //     glm::vec4 position(m_eye.x, m_eye.y, m_eye.z, 1);
    //     glm::vec4 pivot(m_lookAt.x, m_lookAt.y, m_lookAt.z, 1);

    //     // Rotate camera around pivot point
    //     glm::mat4 rx(1.f);
    //     rx = glm::rotate(rx, angleX, m_upVector);
    //     position = (rx * (position - pivot)) + pivot;

    //     glm::mat4 ry(1.f);
    //     ry = glm::rotate(ry, angleY, GetRightVector());
    //     position = (ry * (position - pivot)) + pivot;

    //     m_target_eye = position;
    // }

    void Zoom(float delta, float r, glm::vec3 box) {
        if (m_lock)
            return;
        // Compute SDF from camera to box (source: https://iquilezles.org/articles/distfunctions/)
        // glm::vec3 q = glm::abs(m_eye) - box;
        // float sdf = glm::length(glm::max(q, glm::vec3(0))) + std::fmin(std::fmax(q.x, std::fmax(q.y, q.z)), 0);

        // auto nxt_m_eye = GetViewDir() * delta * 0.2f;
        float sdf = glm::distance(m_eye, box);

        // float d = glm::distance(m_eye, m_lookAt);

        if (sdf < r && delta > 0) {
            delta = 0;
        }

        // float factor = d - r;
        // float factor = sdf - r;

        m_eye += GetViewDir() * delta * 0.2f;
        SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void MoveForward(float speed) {
        if (m_lock)
            return;

        m_eye += GetViewDir() * speed;
        SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    // void MoveUp(float speed) {
    //     if (m_lock)
    //         return;

    //     m_lookAt += m_upVector * speed;
    //     SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    // }

    // void MoveRight(float speed) {
    //     if (m_lock)
    //         return;

    //     m_lookAt += GetRightVector() * speed;
    //     SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    // }

    void MoveUp(float speed) {
        if (m_lock)
            return;

        m_eye += m_upVector * speed;
        m_lookAt += m_upVector * speed;
        SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void MoveRight(float speed) {
        if (m_lock)
            return;

        m_eye += GetRightVector() * speed;
        m_lookAt += GetRightVector() * speed;
        SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }


    // float bell(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
    //     return max_value * std::exp(-std::pow((x - center) / w, 2));
    // }

    float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
        // return 1.f / (1.f + std::exp(-(x - center)));
        return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
    }

    void NewZoom(float delta) {
        float factor = sigmoid(m_fovAndScreen.x, 45.f, 30.f, 2.5f);
        m_fovAndScreen.x = std::clamp(m_fovAndScreen.x + delta * factor, 0.25f, 60.f);

        SetCameraView(m_eye, m_lookAt, m_upVector, m_fovAndScreen);
    }

    void SmoothZoom(float delta) {
        m_target_eye += GetViewDir() * delta;
    }

    glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }
    glm::vec3 lerp(glm::vec4 x, glm::vec4 y, float t) {
        return x * (1.f - t) + y * t;
    }


    void Update(float t) {
        m_eye = lerp(m_eye, m_target_eye, t);
        SetCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void SetLock(bool lock) {
        m_lock = lock;
    }

    bool IsLocked() { return m_lock; }


    glm::mat4x4 GetViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 GetProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 GetEye() const { return m_eye; }
    glm::vec3 GetUpVector() const { return m_upVector; }
    glm::vec3 GetLookAt() const { return m_lookAt; }
    glm::vec3 GetFovAndScreen() const { return m_fovAndScreen; }
    void SetFov(float fov) { m_fovAndScreen.x = fov; NewZoom(0.f);/* TODO here update after set fov... not really smart */ }
    void SetScreenSize(float width, float height) { m_fovAndScreen.y = width; m_fovAndScreen.z = height; }

    glm::vec3 GetViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }
    glm::vec3 GetRightVector() const { return glm::transpose(m_viewMatrix)[0]; }

private:
    glm::vec3 m_fovAndScreen;
    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
    glm::vec3 m_target_eye;
    bool m_lock = false;

};