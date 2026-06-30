#pragma once

#ifdef _WIN32
#include "glew.h"
#else
#include "glad.h"
#endif

#include "vec.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Shader {
    // Program id
    unsigned int id;

    Shader(std::string vertexShaderPath, std::string fragmentShaderPath);

    // Activate shader
    void use();
    // Clean
    void clean();
    
    // utility uniform function
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setFloat2(const std::string &name, sl::algebra::vec2 value) const;
    void setFloat3(const std::string &name, sl::algebra::vec3 value) const;
    void setMat4(const std::string &name, sl::algebra::mat4x4 value) const;
    void setFloat2AtIndex(const std::string &name, int i, sl::algebra::vec2 value) const;
};