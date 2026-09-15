#include "Line.h"
#include "util/Globals.h"

Line::Line(glm::vec3 start, glm::vec3 end) 
    : Shape3D(start, false),
      end(end - start)
{
	shader = Globals::FlatShader;
    invalidateShape();
}

void Line::setCoordinates(glm::vec3 start, glm::vec3 end) {
    this->end = end - start;
    setPosition(start);
    // setPosition already called invalidateModel
    invalidateShape();
}

std::vector<Vertex> Line::generateVertices() {
    return {Vertex{glm::vec3(0.0f)}, Vertex{end}};
}

std::vector<GLuint> Line::generateIndices() {
    return {0, 1};
}

void Line::draw(Camera& camera) {
    preDraw(camera);
    mesh.drawLine(camera, *shader);
}
