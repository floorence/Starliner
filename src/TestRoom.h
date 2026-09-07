#pragma once

#include "3d/shape/RectangularPrism.h"
#include "3d/shape/Sphere.h"
#include "lighting/Light.h"
#include "texture/ImageTexture.h"

class TestRoom {
public:
	std::vector<Drawable3D*> objects;
    Light lightData;

    TestRoom();
private:
    ImageTexture planksDiffuse;
	ImageTexture planksSpecular;
	ImageTexture metalDiffuse;
	ImageTexture metalSpecular;
	ImageTexture stoneDiffuse;
	ImageTexture stoneSpecular;

    Sphere sphere;
    RectangularPrism rect;
    RectangularPrism floor;
    RectangularPrism rightWall;
    RectangularPrism leftWall;
    RectangularPrism floorCube;
    RectangularPrism floorLight;
};