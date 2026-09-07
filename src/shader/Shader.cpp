#include "Shader.h"
#include <fmt/format.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "util/Log.h"

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);

	GLuint vertexShader = createShader(vertexCode.c_str(), ShaderType::Vertex);
	GLuint fragmentShader = createShader(fragmentCode.c_str(), ShaderType::Fragment);

	ID = glCreateProgram();
	// attach the vertex and fragment shaders to the shader program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// link all the shaders together into the shader program
	glLinkProgram(ID);
	logCompileErrors(ID, ShaderType::Program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const char* vertexFile, const char* geometryFile, const char* fragmentFile) {
	std::string vertexCode = getFileContents(vertexFile);
	std::string geometryCode = getFileContents(geometryFile);
	std::string fragmentCode = getFileContents(fragmentFile);

	GLuint vertexShader = createShader(vertexCode.c_str(), ShaderType::Vertex);
	GLuint geometryShader = createShader(geometryCode.c_str(), ShaderType::Geometry);
	GLuint fragmentShader = createShader(fragmentCode.c_str(), ShaderType::Fragment);

	ID = glCreateProgram();
	// attach the vertex and fragment shaders to the shader program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, geometryShader);
	glAttachShader(ID, fragmentShader);
	// link all the shaders together into the shader program
	glLinkProgram(ID);
	logCompileErrors(ID, ShaderType::Program);

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

void Shader::activate() {
	glUseProgram(ID);
}

void Shader::setModel(glm::mat4 model) {
	activate();
	glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
}

void Shader::setCamera(Camera& camera) {
	activate();
	glUniform3f(glGetUniformLocation(ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
	glUniformMatrix4fv(glGetUniformLocation(ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getCameraMatrix()));
}

void Shader::setPointLightCamera(PointLightCamera& camera) {
	activate();
    std::string uniform = "shadowMatrices[0]";

    for (uint i = 0; i < 6; i++) {
        uniform[uniform.size() - 2] = i + '0';
        glUniformMatrix4fv(glGetUniformLocation(ID, uniform.c_str()), 1, GL_FALSE, glm::value_ptr(camera.shadowTransforms[i]));
    }
	setFarPlane(camera.farPlane);
    glUniform3f(glGetUniformLocation(ID, "lightPos"), camera.position.x, camera.position.y, camera.position.z);
}

void Shader::setFarPlane(float farPlane) {
	activate();
    glUniform1f(glGetUniformLocation(ID, "farPlane"), farPlane);
}

void Shader::setTexture(Texture& texture, GLuint unit) {
	activate();

	glUniform1i(glGetUniformLocation(ID, texture.uniform.c_str()), unit);
	texture.bind(unit);
}

void Shader::setShininess(float shininess) {
	activate();
	glUniform1f(glGetUniformLocation(ID, "material.shininess"), shininess);
}

void Shader::setProjection(glm::mat4 projection) {
	activate();
	glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Shader::setExposure(float exposure) {
	activate();
	glUniform1f(glGetUniformLocation(ID, "exposure"), exposure);
}

void Shader::setTextColor(glm::vec3 textColor) {
	activate();
	glUniform3f(glGetUniformLocation(ID, "textColor"), textColor.x, textColor.y, textColor.z);
}

void Shader::setBlurHorizontal(bool horizontal) {
	activate();
	glUniform1i(glGetUniformLocation(ID, "horizontal"), horizontal);
}

void Shader::setBloomEnabled(bool bloomEnabled) {
	activate();
	glUniform1i(glGetUniformLocation(ID, "bloomEnabled"), bloomEnabled);
}

void Shader::setShadowQuality(int quality) {
	activate();
	glUniform1i(glGetUniformLocation(ID, "shadowQuality"), quality);
}

void Shader::setColorSource(ColorSource source) {
	activate();
	glUniform1i(glGetUniformLocation(ID, "colorSource"), static_cast<int>(source));
}

void Shader::setRotation(glm::mat4 rotation) {
	activate();
	glUniformMatrix4fv(glGetUniformLocation(ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation));
}

void Shader::setDeltaTime(float deltaTime) {
	activate();
	glUniform1f(glGetUniformLocation(ID, "deltaTime"), deltaTime);
}

void Shader::registerLightSource(int num, glm::vec3 lightColor, glm::vec3 lightPos, float linear, float quadratic) {
	// Log::log(TAG, fmt::format("registerLightSource, num: {}, color: {}, {}, {}, pos: {}, {}, {}",
	// 	num, lightColor.x, lightColor.y, lightColor.z, lightPos.x, lightPos.y, lightPos.z));
    activate();

    std::string pointLightUniform = "pointLights[0]";
    pointLightUniform[pointLightUniform.size() - 2] = num + '0';
	glUniform3f(glGetUniformLocation(ID, (pointLightUniform + ".color").c_str()), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(ID, (pointLightUniform + ".position").c_str()), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(ID, (pointLightUniform + ".constant").c_str()), 1.0);
	glUniform1f(glGetUniformLocation(ID, (pointLightUniform + ".linear").c_str()), linear);
	glUniform1f(glGetUniformLocation(ID, (pointLightUniform + ".quadratic").c_str()), quadratic);
}

void Shader::setNumPointLights(int num) {
    activate();
	glUniform1i(glGetUniformLocation(ID, "numPointLights"), num);
}

void Shader::setColor(glm::vec3 color) {
	activate();
	glUniform3f(glGetUniformLocation(ID, "materialColor"), color.x, color.y, color.z);
}

void Shader::setColorTint(glm::vec4 color) {
	activate();
	glUniform4f(glGetUniformLocation(ID, "tintColor"), color.x, color.y, color.z, color.a);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

GLuint Shader::createShader(const char* source, ShaderType type) {
	int glShaderType;
	switch (type) {
		case ShaderType::Vertex: glShaderType = GL_VERTEX_SHADER; break;
		case ShaderType::Geometry: glShaderType = GL_GEOMETRY_SHADER; break;
		case ShaderType::Fragment: glShaderType = GL_FRAGMENT_SHADER; break;
		case ShaderType::Program: 
			Log::err(TAG, "createShader() called with ShaderType Program!");
			return 0;
	}
	GLuint shader = glCreateShader(glShaderType);
	// attach shader code to opengl shader object
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	logCompileErrors(shader, type);
	return shader;
}

void Shader::logCompileErrors(unsigned int shader, ShaderType type) {
	GLint hasCompiled;
	char infoLog[1024];

	if (type != ShaderType::Program) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			Log::err(TAG, fmt::format("SHADER_COMPILATION_ERROR for: {}\n{}", typeToString(type), infoLog));
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			Log::err(TAG, fmt::format("SHADER_LINKING_ERROR for: {}\n{}", typeToString(type), infoLog));
		}
	}
}

std::string Shader::typeToString(ShaderType type) {
	switch (type) {
		case ShaderType::Vertex: return "vertex";
		case ShaderType::Geometry: return "geometry";
		case ShaderType::Fragment: return "fragment";
		case ShaderType::Program: return "program";
	}
	return "";
}

std::string Shader::getFileContents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	Log::err(TAG, fmt::format("failed to read shader file: {}", filename));
	throw(errno);
}

Shader::Shader(Shader&& other) noexcept {
	ID = other.ID;
	other.ID = 0;
}
    
Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		glDeleteProgram(ID);
		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}
