#pragma once

#include<glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>;
#include<iostream>


using namespace std;

struct buffer_specs
{
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
	unsigned int num_vertices_to_draw = 0;

	void print()
	{
		cout << VertexArrayID << ", " << vertexbuffer << ", " << uvbuffer << ", " << normalbuffer << ", " << elementbuffer << endl;
	}
};

buffer_specs load_object_to_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices);

void assign_uniform_pointers(GLuint programID);

void Draw_Object(GLFWwindow* window, GLuint shader_program, buffer_specs model_specs, glm::vec3 lightPos, glm::mat4 scale_matrix, glm::mat4 transform_matrix, glm::mat4 rotation_matrix, glm::vec3 Base_Color);

void Cleanup_Object(buffer_specs* object_specs);