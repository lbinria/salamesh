#include "shader.h"

Shader::Shader(const char * vertexShaderPath, const char * fragmentShaderPath, const char * geometryShaderPath) {

    std::string vCode;
    std::string fCode;
    std::string gCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        gShaderFile.open(geometryShaderPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();

        vCode = vShaderStream.str();
        fCode = fShaderStream.str();
        gCode = gShaderStream.str();

    } catch (std::ifstream::failure e) {
        std::cout << "An error occured when reading shader files" << std::endl;
    }

    const char *vShaderCode = vCode.c_str();
    const char *fShaderCode = fCode.c_str();
    const char *gShaderCode = gCode.c_str();

	unsigned int vertexShader, fragmentShader, geometryShader;
    int success;
    char infoLog[512];


    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &gShaderCode, NULL);
    glCompileShader(geometryShader);  

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Failed to compile geometry shader\n" << infoLog << std::endl;
    }

    // Create & compile shader source
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Failed to compile vertex shader\n" << infoLog << std::endl;
    }

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Failed to compile fragment shader\n" << infoLog << std::endl;
    }

	id = glCreateProgram();


	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
    glAttachShader(id, geometryShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Failed to create shader program.\n" << infoLog << std::endl;
	}



	// Clean
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  
	glDeleteShader(geometryShader);

}

Shader::Shader(const char * vertexShaderPath, const char * fragmentShaderPath) {
    std::string vCode;
    std::string fCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vCode = vShaderStream.str();
        fCode = fShaderStream.str();

    } catch (std::ifstream::failure e) {
        std::cout << "An error occured when reading shader files" << std::endl;
    }

    const char *vShaderCode = vCode.c_str();
    const char *fShaderCode = fCode.c_str();

	unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    // Create & compile shader source
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Failed to compile vertex shader\n" << infoLog << std::endl;
    }

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Failed to compile fragment shader\n" << infoLog << std::endl;
    }

	id = glCreateProgram();


	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Failed to create shader program.\n" << infoLog << std::endl;
	}



	// Clean
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  

}

void Shader::use() {
    glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat2(const std::string &name, glm::vec2 value) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

void Shader::setFloat3(const std::string &name, glm::vec3 value) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::clean() {
    glDeleteProgram(id);
}