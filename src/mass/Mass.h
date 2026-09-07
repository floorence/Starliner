#pragma once

#include <glm/glm.hpp>
#include <string>

class Mass {
public: 
    float mass = 0.0f; // kg
	glm::vec3 position = glm::vec3(0.0f); // m
	glm::vec3 velocity = glm::vec3(0.0f); // m/s
    glm::vec3 acceleration = glm::vec3(0.0f); // m/s^2

	Mass() = default;

	void applyForce(glm::vec3 f); // DOES NOT actually move the mass. call updatePosition to do so.

	/** @return vector from initial to final position */
    glm::vec3 updatePosition(float dt);
	virtual std::string getDebugString();

    virtual ~Mass() = default;
protected:
	const float AIR_RESISTANCE_COEFFICIENT = 1.0f;
	const float STOPPING_SPEED = 0.0005f;

	glm::vec3 prevAcceleration; // used to access acceleration of previous frame, or acceleration of current frame after calling updatePosition

	glm::vec3 getAirResistance(); // return force from air resistance
	virtual void processVelocity();
};
