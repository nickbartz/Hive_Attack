#include <stdio.h>
#include <stdlib.h>
#include<vector>
#include<map>
#include<iostream>

#include<Global_Render_Variables.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Service_Locator.h>
#include<Render_Manager.h>
#include<Gameplay_Manager.h>
#include<Memory_Manager.h>


using namespace std;
using namespace glm;

Service_Locator service_locator;
Memory_Manager memory_manager;
Gameplay_Manager gameplay_manager;
Render_Manager render_manager;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	render_manager.handle_mouse_input(window, button, action, mods);
}

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initalize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	GLFWwindow* window;

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Open_GL_Test", NULL, NULL);
	glfwSetWindowPos(window, WINDOW_OFFSET_X, WINDOW_OFFSET_Y);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to open window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed ot initalize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	service_locator.register_service(&memory_manager);
	service_locator.register_service(&render_manager);
	service_locator.register_service(&gameplay_manager);

	memory_manager.Init(&service_locator);
	render_manager.Init(&service_locator);
	gameplay_manager.Init(&service_locator);
	
	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	float addition = 0.1;

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	srand(50);

	double x_pos, y_pos;

	do {

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
											// printf and reset
			printf("%f ms/frame, %f FPS\n", 1000.0 /double(nbFrames), double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSetMouseButtonCallback(window, mouse_button_callback);

		// Update
		gameplay_manager.update_scene_graph();

		// Draw
		render_manager.computeMatricesFromInputs(window);

		gameplay_manager.draw_scene_graph(window);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	gameplay_manager.cleanup_scene_graph();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}