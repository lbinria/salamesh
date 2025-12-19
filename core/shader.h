#pragma once

#ifdef _WIN32
#include "../include/glew/include/GL/glew.h"
#else
#include "../include/glad/glad.h"
#endif


  
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

struct Shader {
    // Program id
    unsigned int id;

    Shader(const char * vertexShaderPath, const char * fragmentShaderPath);
    Shader(const char * vertexShaderPath, const char * fragmentShaderPath, const char * geometryShaderPath);

    // Activate shader
    void use();
    // Clean
    void clean();
    
    // utility uniform function
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setFloat2(const std::string &name, glm::vec2 value) const;
    void setFloat3(const std::string &name, glm::vec3 value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setFloat2AtIndex(const std::string &name, int i, glm::vec2 value) const;
};