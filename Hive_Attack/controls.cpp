#include<controls.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform.hpp>

#include<iostream>
using namespace std;


float speed = 24.0f; // 3 units / second
float mouseSpeed = 0.0005f;

// Matrices
float FoV = 45.0f;
glm::mat4 projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
glm::mat4 View;

glm::vec3 position = glm::vec3(0, 12, 20);
glm::vec3 direction = glm::vec3(0.0,0.0,0.0);
glm::vec3 right_vector = { 0.0f, 0.0f, 0.0f };
glm::vec3 up = { 0.0f, 0.0f, 0.0f };

// horizontal angle : toward -Z
float horizontalAngle = 3.1459;
// vertical angle : 0, look at the horizon
float verticalAngle = -0.6f;

double currentTime;
float deltaTime;

glm::mat4 getProjectionMatrix()
{
	return glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
}

glm::mat4 getViewMatrix()
{
	return glm::lookAt(position, direction + position, up);
}

void computeMatricesFromInputs(GLFWwindow* window)
{
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	// Position Vector
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.x += direction.x * deltaTime * speed;
		position.z += direction.z * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.x -= direction.x * deltaTime * speed;
		position.z -= direction.z * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right_vector * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right_vector * deltaTime * speed;
	}

	// Move North 
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}

	// Move South 
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}


	// Direction Vector
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle));

	// Right Vector
	right_vector = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f));

	// Up Vector
	up = glm::cross(right_vector, direction);

	lastTime = currentTime;
}

