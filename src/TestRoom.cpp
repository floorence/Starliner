#include "TestRoom.h"
#include "3d/shape/RectangularPrism.h"
#include "3d/shape/Sphere.h"

TestRoom::TestRoom() 
    : planksDiffuse("assets/planks.png", TextureType::Diffuse),
      planksSpecular("assets/planks.png", TextureType::Specular, GL_UNSIGNED_BYTE, true),
      metalDiffuse("assets/metal.jpg", TextureType::Diffuse),
      metalSpecular("assets/metal.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true),
      stoneDiffuse("assets/stone.jpg", TextureType::Diffuse),
      stoneSpecular("assets/stone.jpg", TextureType::Specular, GL_UNSIGNED_BYTE, true),
      sphere(&planksDiffuse, &planksSpecular, glm::vec3(2.0f, 0.0f, 0.0f), 0.2f),
      rect(&metalDiffuse, &metalSpecular, glm::vec3(-2.0f, 0.0f, 0.0f), 0.5f, 1.0f, 0.75f),
      floor(&stoneDiffuse, &stoneSpecular, glm::vec3(0.0f, -1.2f, 0.0f), 5.0f, 0.1f, 5.0f),
	  rightWall(&stoneDiffuse, &stoneSpecular, glm::vec3(2.5f, 1.3f, 0.0f), 0.1f, 5.0f, 5.0f),
	  leftWall(&stoneDiffuse, &stoneSpecular, glm::vec3(-2.5f, 1.3f, 0.0f), 0.1f, 5.0f, 5.0f),
      floorCube(&planksDiffuse, &planksSpecular, glm::vec3(-1.0f, -0.95f, -1.0f), 0.4f, 0.4f, 0.4f),
      floorLight(glm::vec3(0.0f, -0.8f, 0.0f), 0.2f, 0.2f, 0.2f, true)
{
	// DebugPyramid pyramid(&planksDiffuse, &planksSpecular, glm::vec3(0.0f, 0.0f, 0.0f));
	// objects.push_back(&pyramid);

	rect.setRotation(0, 0, glm::radians(180.0f));
	floorLight.setColor(glm::vec3(100.0f, 100.0f, 100.0f));
	lightData = Light(10, floorLight.getPosition(), floorLight.getColor());

	// RectangularPrism backWall(&stoneDiffuse, &stoneSpecular, glm::vec3(0.0f, 1.3f, 2.5f), 5.0f, 5.0f, 0.1f);
	// RectangularPrism frontWall(&stoneDiffuse, &stoneSpecular, glm::vec3(0.0f, 1.3f, -2.5f), 5.0f, 5.0f, 0.1f);

	// objects.push_back(&backWall);
	// objects.push_back(&frontWall);

	// RectangularPrism light(nullptr, nullptr, glm::vec3(0.5f, 0.5f, 0.5f), 0.2f, 0.2f, 0.2f, true);
	// light.setColor(glm::vec3(1.0f, 1.0f, 1.0f), 7.0f);
	// objects.push_back(&light);

    objects = {&sphere, &rect, &floor, &rightWall, &leftWall, &floorCube, &floorLight};
}
