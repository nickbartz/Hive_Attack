#pragma once

#include<glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>;
#include<iostream>

using namespace std;
using namespace glm;

struct loaded_model_buffer
{
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
	unsigned int num_indices = 0;
};

struct model_buffer_specs
{
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
	GLuint model_matrix_instance_buffer;
	unsigned int num_indices = 0;

	void print()
	{
		cout << VertexArrayID << ", " << vertexbuffer << ", " << uvbuffer << ", " << normalbuffer << ", " << elementbuffer << endl;
	}
};

struct instanced_buffer_specs
{
	GLuint ship_model_matrix_buffer;
	int max_objects_in_instance = 0;
};

loaded_model_buffer load_model_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices);

model_buffer_specs create_object_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices);

void assign_uniform_pointers(GLuint programID, GLuint Instance_render_shader);

void Draw_Object(GLFWwindow* window, GLuint shader_program, model_buffer_specs model_specs, glm::vec3 lightPos, glm::mat4 model_matrix, glm::vec3 Base_Color);

void Draw_Instanced_Object(GLFWwindow * window, GLuint shader_program, glm::vec3 lightPos, model_buffer_specs model_specs, int num_items_to_render, vec3 Base_Color, mat4* swarm_model_matrices);

void Cleanup_Object(model_buffer_specs* object_specs);

