#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>

#include "TestRoom.h"
#include "gui/Hud.h"
#include "window/Window.h"
#include "gui/framework/ClickController.h"
#include "gui/SettingsMenu.h"
#include "lighting/LightController.h"
#include "texture/FontTexture.h"
#include "util/Globals.h"
#include "util/Log.h"
#include "mass/Player.h"
#include "world/World.h"

// initial window dimensions, which might not match what will be loaded from save
const unsigned int width = 800;
const unsigned int height = 600;
const std::string TAG = "Main";

Player* playerPtr;
ClickController* clickControllerPtr;
LightController* lightControllerPtr;
Window* windowPtr;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	playerPtr->handleMousePos(window, xpos, ypos);
	clickControllerPtr->handleMousePos(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	playerPtr->handleMouseScroll(window, xoffset, yoffset);
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
	playerPtr->handleKeyInputs(window, key, action);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
		clickControllerPtr->handleMouseButton(xpos, ypos, action);
    }
}

void windowSizeCallback(GLFWwindow*, int width, int height) {
    Log::log(TAG, fmt::format("Window size changed: {}x{}", width, height));
	windowPtr->setSizeAndNotify(width, height);
}

void frameBufferSizeCallback(GLFWwindow*, int width, int height) {
    Log::log(TAG, fmt::format("Framebuffer size changed: {}x{}", width, height));
	lightControllerPtr->onFrameBufferSizeChanged(width, height);
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(width, height, "window", nullptr, nullptr);

	if (window == nullptr) {
		Log::err(TAG, "Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	// on some displays, framebuffer size and window size are not always the same
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);  // enable VSync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	Window w(window);

	gladLoadGL();

	Log::log(TAG, "Window and OpenGL initialized");

	// create shaders, textures, and populate globals
	Shader shader("shader/default.vert", "shader/default.frag");
	Shader lightShader("shader/light.vert", "shader/light.frag");
	Shader flatShader("shader/default.vert", "shader/gui.frag");
	Shader guiShader("shader/gui.vert", "shader/gui.frag");
	Shader fontShader("shader/gui.vert", "shader/font.frag");
	glm::mat4 guiProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);	
	guiShader.setProjection(guiProjection);
	fontShader.setProjection(guiProjection);

	Log::log(TAG, "shaders initialized");

	FontTexture fontTex("assets/pixel_operator_short_dollar.ttf");

	Log::log(TAG, "textures initialized");

	Globals::Font = &fontTex;
	Globals::DefaultShader = &shader;
	Globals::LightShader = &lightShader;
	Globals::FlatShader = &flatShader;
	Globals::GuiShader = &guiShader;
	Globals::FontShader = &fontShader;

	TestRoom testRoom;
	Log::log(TAG, "Test room initialized");

	Player player(glm::vec3(0.0f, 0.0f, 2.0f), width, height);

	Text debugText;
	debugText.setBounds(10, 10, 400, 200);
	debugText.setFontSize(20);
	debugText.setCenterText(false);

	SettingsController sc;
	SettingsMenu settingsMenu(&sc);
	settingsMenu.setCorners(100, 100, width - 100, height - 100);

	Hud hud(width, height, &settingsMenu);

	LightController lc(fbWidth, fbHeight);
	ClickController cc;

	World world(67, &lc);

	// set pointers used in glfw callbacks
	windowPtr = &w;
	playerPtr = &player;
	lightControllerPtr = &lc;
	clickControllerPtr = &cc;

	// register listeners, shapes, and drawables
	w.registerListeners({&hud, &player, &settingsMenu});

	lc.registerDrawables(testRoom.objects);
	lc.registerLight(testRoom.lightData);
	lc.registerDrawable(&player);
	lc.registerDrawable(&world);

	sc.registerListeners({&settingsMenu, &lc, &player, &w});
	cc.registerListeners({&hud, &settingsMenu});
	
	lc.processLighting();
	Log::log(TAG, "initial lighting processing completed");

	sc.load();
	Log::log(TAG, "settings loaded from save");

	glEnable(GL_CULL_FACE); // enable back face culling

	Log::log(TAG, "everything is set up; starting main game loop");

	while (!glfwWindowShouldClose(window)) {
		w.startFrame();

		player.handleKeyInputs(window, w.deltaTime);
		world.onPlayerPosition(player.position);
		world.update(w.deltaTime);

		glEnable(GL_DEPTH_TEST); // enable depth buffer so that stuff in front blocks stuff behind it
		lc.render(*player.getActiveCamera(), w.deltaTime);
		glDisable(GL_DEPTH_TEST); // disable for gui drawing

		debugText.setText(world.getDebugString());
		debugText.draw();
		hud.setPerformanceText(w.performanceInfo);
		hud.draw();

		if (settingsMenu.isOpen) settingsMenu.draw();

		glfwPollEvents();
		// end frame here since glfwSwapBuffers() suspends when using vsync
		w.endFrame();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}