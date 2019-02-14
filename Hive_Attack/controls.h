#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Global_Render_Variables.h>

class Gameplay_Manager;

glm::mat4 getProjectionMatrix();

glm::mat4 getViewMatrix();

void computeMatricesFromInputs(GLFWwindow* window);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void set_gameplay_manager_temp_func(Gameplay_Manager* gameplay);