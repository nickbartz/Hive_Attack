#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>


void initText2D(const char * texturePath);
void printText2D(const char * text, int x, int y, int size);
void cleanupText2D();