#include "Player.h"
#include "util/Log.h"
#include "util/Utils.h"

Player::Player(glm::vec3 position, int windowWidth, int windowHeight) 
    : camera(position, windowWidth, windowHeight),
	  thirdPersonCam(position, windowWidth, windowHeight),
	  body("assets/models/spaceship/spaceship.obj", position),
	  leftTrail(glm::vec3(0.0f), 10, 0.09f),
	  rightTrail(glm::vec3(0.0f), 10, 0.09f)
{
    this->position = position;
    camera.setPerspective(45.0f, 0.1f, 100.0f);
    thirdPersonCam.setPerspective(45.0f, 0.1f, 100.0f);
	mass = 1.0f;

	lastX = camera.windowWidth / 2.0;
	lastY = camera.windowHeight / 2.0;

	body.setScale(1.0f / 600.0f);
	body.setDefaultRotation(0.0f, glm::radians(180.0f), 0.0f);

	orientationLine.setColor(glm::vec3(100.0f, 0.0f, 69.0f));

	leftTrail.setColor(glm::vec3(10.0f, 4.0f, 0.0f));
	rightTrail.setColor(glm::vec3(10.0f, 4.0f, 0.0f));

	thirdPersonCam.position = glm::vec3(position.x, position.y, position.z + thirdPersonDist);
	syncCamerasAndBody(glm::vec3(0.0f), 0.0f, 0.0f, 0.0f);
}

Camera* Player::getActiveCamera() {
	return (thirdPerson) ? &thirdPersonCam : &camera;
}

glm::vec3* Player::getActiveOrientation() {
	return (thirdPerson) ? &thirdPersonOrientation : &orientation;
}

std::string Player::getDebugString() {
	return fmt::format("position: {:.3f}, {:.3f}, {:.3f}\nthirdPerson: {:.3f}, {:.3f}, {:.3f}",
        position.x, position.y, position.z,
		thirdPersonCam.position.x, thirdPersonCam.position.y, thirdPersonCam.position.z
	);
}

void Player::handleKeyInputs(GLFWwindow* window, float deltaTime) {
	if (!focused) return;

	glm::vec3 force = glm::vec3(0.0, 0.0, 0.0); // Newtons
	int yawTurn = 0;
	int pitchTurn = 0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (thirdPerson)
			force += orientation;
		else
			force += glm::normalize(glm::vec3(orientation.x, 0.0f, orientation.z));

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		yawTurn++;
		if (!thirdPerson) force += -glm::normalize(glm::cross(orientation, Constants::UP));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (thirdPerson) {
			float forwardSpeed = glm::dot(velocity, orientation);
			if (forwardSpeed > 0) force -= orientation;
		} else
			force -= glm::normalize(glm::vec3(orientation.x, 0.0f, orientation.z));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		yawTurn--;
		if (!thirdPerson) force += glm::normalize(glm::cross(orientation, Constants::UP));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		pitchTurn--;
		if (!thirdPerson) force += Constants::UP;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		pitchTurn++;
		if (!thirdPerson) force -= Constants::UP;
	}

	if (force != glm::vec3(0.0, 0.0, 0.0)) {
		force = FORCE_MULTIPLIER * glm::normalize(force);
		applyForce(force);
	}
	glm::vec3 movement = updatePosition(deltaTime);
	syncCamerasAndBody(movement, yawTurn, pitchTurn, deltaTime);
}

void Player::handleKeyInputs(GLFWwindow* window, int key, int action) {
	if (key == GLFW_KEY_E) {
		if (action == GLFW_PRESS) handleFocusChange(window);
	}
	if (key == GLFW_KEY_P) {
		if (action == GLFW_PRESS) thirdPerson = !thirdPerson;
	}
}

void Player::handleMousePos(GLFWwindow*, double xpos, double ypos) {
	if (!focused) return;

	if (firstClick) {
		lastX = xpos;
		lastY = ypos;
		firstClick = false;
	}

	double xOffset = xpos - lastX;
	double yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	float rotX = sensitivity * (float)(xOffset) / camera.windowWidth;
	float rotY = sensitivity * (float)(yOffset) / camera.windowHeight;

	// calculate upcoming vertical change in the orientation
	glm::vec3 newOrientation = *getActiveOrientation();
	glm::vec3 verticalOrientation = glm::normalize(glm::rotate(
		newOrientation,
		glm::radians(-rotY),
		glm::normalize(glm::cross(newOrientation, Constants::UP))
	));

	// decide whether or not the next vertical orientation is legal or not
	if (std::abs(glm::angle(verticalOrientation, Constants::UP) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
		newOrientation = verticalOrientation;
	}

	// rotate the orientation left and right
	newOrientation = glm::rotate(newOrientation, glm::radians(-rotX), Constants::UP);
	
	if (thirdPerson) {
		thirdPersonCam.position = position - Utils::setVectorLength(newOrientation, thirdPersonDist);
	} else {
		orientation = newOrientation;
	}

	// lookAt is already called every frame in syncCamerasAndBody()
}

void Player::handleMouseScroll(GLFWwindow*, double, double yoffset) {
	// scroll down = zoom in. yoffset is positive when scroll down i think
 
	if (focused && thirdPerson) {
		thirdPersonDist = std::max(thirdPersonDist - (float)yoffset, MIN_THIRD_PERSON_DIST);
		thirdPersonCam.position = position - Utils::setVectorLength(thirdPersonOrientation, thirdPersonDist);
	}
}

void Player::handleFocusChange(GLFWwindow* window) {
	if (focused) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		focused = false;
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstClick = true;
		focused = true;
	}
}

void Player::onSettingsChanged(const Settings& settings) {
	sensitivity = settings.controls.sensitivity.value;
}

void Player::onWindowSizeChanged(int newWidth, int newHeight) {
	camera.setViewSize(newWidth, newHeight);
	thirdPersonCam.setViewSize(newWidth, newHeight);
}

void Player::draw(Camera& camera) {
	if (thirdPerson) {
		body.draw(camera);
		orientationLine.draw(camera);
		leftTrail.draw(camera);
		rightTrail.draw(camera);
	}
}

void Player::drawToDepthMap(PointLightCamera& camera, Shader& depthShader) {
	if (thirdPerson) body.drawToDepthMap(camera, depthShader);
}

void Player::syncCamerasAndBody(glm::vec3 movement, int yawTurn, int pitchTurn, float deltaTime) {
	camera.position = this->position;
	camera.lookAt(orientation);

	thirdPersonCam.position += movement;
	thirdPersonOrientation = position - thirdPersonCam.position;
	thirdPersonCam.lookAt(thirdPersonOrientation);

	body.setPosition(position);

	if (thirdPerson) {
		float tiltAmount = TILT_TURN_SPEED * deltaTime;
		if (yawTurn == 0) {
			roll = Utils::approach(roll, 0.0f, tiltAmount);
		} else {
			roll = std::clamp(roll + yawTurn * tiltAmount, -TILT_MAX, TILT_MAX);
		}
		if (pitchTurn == 0) {
			pitch = Utils::approach(pitch, 0.0f, tiltAmount);
		} else {
			pitch = std::clamp(pitch + pitchTurn * tiltAmount, -TILT_MAX, TILT_MAX);
		}

		yaw += yawTurn * tiltAmount;
		// pitch += pitchTurn * tiltAmount;
		// Log::log("Player", fmt::format("tiltAmount: {}, turnAmount: {}", tiltAmount, turnAmount));

		orientation = glm::rotate(Constants::FORWARD, yaw, Constants::UP);
		glm::vec3 left = glm::normalize(glm::cross(Constants::UP, orientation)); // not taking into account roll, since that's how pitch works
		orientation = glm::rotate(orientation, pitch, left);
		// glm::vec3 up = glm::normalize(glm::cross(orientation, left));

		body.setRotation(yaw, Constants::UP);
		// use local axes, otherwise rotation will be post multiplied against existing rotation
		body.rotate(pitch, Constants::RIGHT); // instead of left since body has default rotation of 180 degrees
		body.rotate(roll, Constants::FORWARD);

		timeSinceLastPoint += deltaTime;
		if (timeSinceLastPoint >= TRAIL_POINT_PERIOD) {
			// use negative roll since body rotation is reversed due to it being rotated 180 degrees
			left = glm::rotate(left, -roll, orientation);
			glm::vec3 dimens = body.getDimensions();
			glm::vec3 posToBack = Utils::setVectorLength(-orientation, dimens.z / 2.5f);
			glm::vec3 backToLeftThruster = Utils::setVectorLength(left, dimens.x / 7.9f);
			leftTrail.addPointWithDir(position + posToBack + backToLeftThruster, -roll, orientation);
			rightTrail.addPointWithDir(position + posToBack - backToLeftThruster, -roll, orientation);
			timeSinceLastPoint = 0.0f;
		}
	}

	orientationLine.setCoordinates(position, position + orientation);
}
