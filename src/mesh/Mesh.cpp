#include "Mesh.h"
#include "util/Log.h"
#include "util/Utils.h"

Mesh::Mesh(
	const std::vector <Vertex>& vertices, 
	const std::vector <GLuint>& indices, 
	const std::vector<Texture*>& textures
) {
	setTextures(textures);
	setShapeData(vertices, indices);
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector <GLuint>& indices, const glm::vec3 color) {
	setColor(color);
	setShapeData(vertices, indices);
}

glm::vec3 Mesh::getColor() {
	return material.color;
}

void Mesh::setColor(const glm::vec3 color) {
	material.color = color;
	material.colorSource = ColorSource::MaterialColor;
}

void Mesh::setTextures(const std::vector<Texture*>& textures) {
	material.textures = textures;
	material.colorSource = ColorSource::Texture;
}

void Mesh::setShapeData(const std::vector <Vertex>& vertices, const std::vector <GLuint>& indices) {
	drawCount = indices.size();

	vao.bind();
	vbo.linkVertices(vertices);
	ebo.linkIndices(indices);
	// link vertex data from vbo to vao
	vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	vao.linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	vao.linkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	vao.linkAttrib(vbo, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(8 * sizeof(float)));

	Utils::unbindVao();
	Utils::unbindVbo();
	Utils::unbindEbo();
}

void Mesh::draw(Camera& camera, Shader& shader) {
	shader.activate(); // bind shader to be able to access uniforms
	vao.bind();

	prepareColorsAndTextures(shader);
	shader.setCamera(camera);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawGui(Shader& shader) {
	shader.activate(); // bind shader to be able to access uniforms
	vao.bind();

	prepareColorsAndTextures(shader);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawLine(Camera& camera, Shader& shader) {
	shader.activate(); // bind shader to be able to access uniforms
	vao.bind();

	prepareColorsAndTextures(shader);
	shader.setCamera(camera);
	glDrawElements(GL_LINES, drawCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawToDepthMap(PointLightCamera& camera, Shader& depthShader) {
	depthShader.activate();
	vao.bind();

	// depth map does not have textures or colours

	depthShader.setPointLightCamera(camera);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
}

void Mesh::prepareColorsAndTextures(Shader& shader) {
	shader.setColorSource(material.colorSource);
	if (material.colorSource == ColorSource::Texture) {
		for (uint i = 0; i < material.textures.size(); i++) {
			shader.setTexture(*material.textures[i], i);
		}
	} else if (material.colorSource == ColorSource::MaterialColor) {
		shader.setColor(material.color);
	}
}