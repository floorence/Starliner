#pragma once

#include "mass/Mass.h"
#include "window/WindowListener.h"
#include "camera/Camera.h"
#include "settings/SettingsListener.h"
#include "3d/Model.h"
#include "3d/shape/Line.h"
#include "3d/shape/Trail.h"

class Player: public Mass, public SettingsListener, public Drawable3D, public WindowListener {
public:
    Player(glm::vec3 position, int windowWidth, int windowHeight);

	Camera* getActiveCamera();
	std::string getDebugString() override;
	
	// inputs
	void handleKeyInputs(GLFWwindow* window, float deltaTime);
	void handleKeyInputs(GLFWwindow* window, int key, int action);
	void handleMousePos(GLFWwindow* window, double xpos, double ypos);
	void handleMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

    void onSettingsChanged(const Settings& settings) override;
	void onWindowSizeChanged(int newWidth, int newHeight) override;

	void draw(Camera& camera) override;
	void drawToDepthMap(PointLightCamera& camera, Shader& depthShader) override;
private:
	const float FORCE_MULTIPLIER = 5.0f;
	const float TILT_TURN_SPEED = glm::radians(45.0f); // radians per second
	const float TILT_MAX = glm::radians(45.0f); // radians
	const float TRAIL_POINT_PERIOD = 1.0f / 70.0f; // seconds. not using 1/60 since will be weird if vsync is on
	float timeSinceLastPoint = 0.0f; // seconds since last addPoint call

	float yaw = 0.0f; // radians
	float pitch = 0.0f; // radians
	float roll = 0.0f; // radians

    Camera camera;
	Camera thirdPersonCam;
	Model body;
	Line orientationLine;
	Trail leftTrail;
	Trail rightTrail;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f); // should always be normalized!!!
	glm::vec3 thirdPersonOrientation = glm::vec3(0.0f, 0.0f, -1.0f); // length should be the same as thirdPersonDist

	bool firstClick = true;
	bool focused = true;
	double lastX, lastY;
	float sensitivity = 100.0f; 

	bool thirdPerson = false;
	float thirdPersonDist = 1.0f; // distance from third person cam to player
	const float MIN_THIRD_PERSON_DIST = 0.2f;

	void handleFocusChange(GLFWwindow* window);
	glm::vec3* getActiveOrientation();
	/** yawTurn and pitchTurn are 0, -1, or 1 depending on direction */
	void syncCamerasAndBody(glm::vec3 movement, int yawTurn, int pitchTurn, float deltaTime);
};
