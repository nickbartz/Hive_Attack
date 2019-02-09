#include<controls.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<Scene_Graph.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform.hpp>
#include<iostream>

using namespace std;


float speed = 24.0f; // 3 units / second
float mouseSpeed = 0.0005f;

// Matrices
float FoV = 45.0f;
glm::mat4 projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
glm::mat4 View;

glm::vec3 position = glm::vec3(0, 25, 40);
glm::vec3 direction = glm::vec3(0.0,0.0,0.0);
glm::vec3 right_vector = { 0.0f, 0.0f, 0.0f };
glm::vec3 up = { 0.0f, 0.0f, 0.0f };

// horizontal angle : toward -Z
float horizontalAngle = 3.14158;
// vertical angle : 0, look at the horizon
float verticalAngle = -0.6f;

double currentTime;
float deltaTime;

glm::mat4 getProjectionMatrix()
{
	return glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
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

	//// Get mouse position
	//double xpos, ypos;
	//glfwGetCursorPos(window, &xpos, &ypos);

	//// Reset mouse position for next frame
	//glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	//horizontalAngle += mouseSpeed * float(WINDOW_WIDTH / 2 - xpos);
	//verticalAngle += mouseSpeed * float(WINDOW_HEIGHT / 2 - ypos);

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		float x = (2.0f * xpos) / WINDOW_WIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / WINDOW_HEIGHT;
		float z = 1.0f;

		glm::vec3 ray_nds = glm::vec3(x, y, z);

		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

		glm::vec4 ray_eye = glm::inverse(getProjectionMatrix()) * ray_clip;

		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

		glm::vec3 ray_wor = (inverse(getViewMatrix()) * ray_eye);
		ray_wor = glm::normalize(ray_wor);

		float scalar = -position.y / ray_wor.y;

		glm::vec3 new_point = position + ray_wor*scalar;

		cout << new_point.x << ", " << new_point.y << ", " << new_point.z << endl;

		Handle_Mouse_Click(new_point.x, new_point.z);
	}
}
