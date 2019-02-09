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

struct model_buffer_specs
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

struct instanced_buffer_specs
{
	GLuint ship_transform_buffer;
	GLuint ship_rotation_buffer;
	int max_objects_in_instance = 0;
};

model_buffer_specs load_object_to_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices);

instanced_buffer_specs load_instance_buffers(int max_ships_in_swarm);

void assign_uniform_pointers(GLuint programID, GLuint Instance_render_shader);

void Draw_Object(GLFWwindow* window, GLuint shader_program, model_buffer_specs model_specs, glm::vec3 lightPos, glm::mat4 scale_matrix, glm::mat4 transform_matrix, glm::mat4 rotation_matrix, glm::vec3 Base_Color);

void Draw_Instanced_Object(GLFWwindow * window, GLuint shader_program, glm::vec3 lightPos, model_buffer_specs model_specs, instanced_buffer_specs instance_specs, int num_items_to_render, vec3 Base_Color, vec3 * swarm_transforms, vec3 * swarm_rotations);

void Cleanup_Object(model_buffer_specs* object_specs);

