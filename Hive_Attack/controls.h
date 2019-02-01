#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

glm::mat4 getProjectionMatrix();

glm::mat4 getViewMatrix();

void computeMatricesFromInputs(GLFWwindow* window);