#pragma once

#include "camera/Camera.h"
#include "camera/PointLightCamera.h"
#include "texture/CubeMapTexture.h"
#include "texture/Texture.h"
#include <glm/glm.hpp>
#include <string>

enum class ShaderType {
	Vertex, Geometry, Fragment, Program
};

enum class ColorSource {
	Texture, VertexColor, MaterialColor
};

class Shader {
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader(const char* vertexFile, const char* geometryFile, const char* fragmentFile);
	~Shader();

	Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

	void activate();
	void setModel(glm::mat4 model); // default.vert
	void setCamera(Camera& camera); // default.vert
	void setPointLightCamera(PointLightCamera& camera); // depth.geom, depth.frag
	void setFarPlane(float farPlane); // default.frag
	void setTexture(Texture& texture, GLuint unit); // default.frag, gui.frag, blur.frag, exposure.frag, hdr_bloom.frag
	void setShininess(float shininess); // default.frag
	void setView(glm::mat4 view); // skybox.vert
	void setProjection(glm::mat4 projection); // gui.vert, skybox.vert
	void setExposure(float exposure); // hdr_bloom.frag
	void setTextColor(glm::vec3 textColor); // font.frag
	void setBlurHorizontal(bool horizontal); // blur.frag
	void setBloomEnabled(bool bloomEnabled); // hdr_bloom.frag
	void setShadowQuality(int quality); // default.frag
	void setColorSource(ColorSource source); // default.frag, gui.frag
	void setRotation(glm::mat4 rotation); // default.vert
	void setDeltaTime(float deltaTime); // exposure.frag

    void registerLightSource(int num, glm::vec3 lightColor, glm::vec3 lightPos, float linear, float quadratic); // default.frag
    void setNumPointLights(int num); // default.frag

	void setColor(glm::vec4 color); // default.frag, light.frag, gui.frag
	void setColorTint(glm::vec4 color); // default.frag, gui.frag
private:
	const std::string TAG = "Shader";
	GLuint createShader(const char* source, ShaderType type);
	void logCompileErrors(unsigned int shader, ShaderType type);
	std::string typeToString(ShaderType type);
	std::string getFileContents(const char* filename);
};
