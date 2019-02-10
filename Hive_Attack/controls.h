#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Global_Render_Variables.h>


glm::mat4 getProjectionMatrix();

glm::mat4 getViewMatrix();

void computeMatricesFromInputs(GLFWwindow* window);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
