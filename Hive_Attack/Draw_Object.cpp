#include<Draw_Object.h>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include<vector>
#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include<controls.h>
#include<texture.hpp>

using namespace std;
using namespace glm;

// Get a handle for our "MVP" uniform
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint TextureID;
GLuint LightID;
GLuint ColorID;

GLuint MatrixID_Instance;
GLuint ViewMatrixID_Instance;
GLuint ModelMatrixID_Instance;
GLuint TextureID_Instance;
GLuint LightID_Instance;
GLuint ColorID_Instance;

GLuint Texture;

model_buffer_specs load_object_to_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices)
{
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// Bind our texture in Texture Unit 0
	Texture = loadBMP_custom("Cell_Texture.bmp");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	return { VertexArrayID, vertexbuffer, uvbuffer, normalbuffer, elementbuffer, indices.size() };
}

instanced_buffer_specs load_instance_buffers(int max_ships_in_swarm)
{
	GLuint ship_transforms_buffer;
	glGenBuffers(1, &ship_transforms_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ship_transforms_buffer);
	glBufferData(GL_ARRAY_BUFFER, max_ships_in_swarm * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);

	GLuint ship_rotations_buffer;
	glGenBuffers(1, &ship_rotations_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ship_rotations_buffer);

	glBufferData(GL_ARRAY_BUFFER, max_ships_in_swarm * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);

	instanced_buffer_specs new_instance_specs;
	new_instance_specs.ship_transform_buffer = ship_transforms_buffer;
	new_instance_specs.ship_rotation_buffer = ship_rotations_buffer;
	new_instance_specs.max_objects_in_instance = max_ships_in_swarm;

	return new_instance_specs;
}

void assign_uniform_pointers(GLuint programID, GLuint instance_render_program)
{
	glUseProgram(programID);
	
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	ColorID = glGetUniformLocation(programID, "BaseColor");

	glUseProgram(instance_render_program);

	MatrixID_Instance = glGetUniformLocation(programID, "MVP");
	ViewMatrixID_Instance = glGetUniformLocation(programID, "V");
	ModelMatrixID_Instance = glGetUniformLocation(programID, "M");
	TextureID_Instance = glGetUniformLocation(programID, "myTextureSampler");
	LightID_Instance = glGetUniformLocation(programID, "LightPosition_worldspace");
	ColorID_Instance = glGetUniformLocation(programID, "BaseColor");
}

void stream_instance_data_to_buffers(instanced_buffer_specs instance, int num_objects_to_render, vec3* swarm_transforms, vec3* swarm_rotations)
{
	glBindBuffer(GL_ARRAY_BUFFER, instance.ship_transform_buffer);
	glBufferData(GL_ARRAY_BUFFER, instance.max_objects_in_instance * sizeof(glm::vec3), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_objects_to_render * sizeof(glm::vec3), swarm_transforms);

	glBindBuffer(GL_ARRAY_BUFFER, instance.ship_rotation_buffer);
	glBufferData(GL_ARRAY_BUFFER, instance.max_objects_in_instance * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);  // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_objects_to_render * sizeof(glm::vec3), swarm_rotations);
}

void Enable_Model_Buffers(model_buffer_specs model_specs)
{
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, model_specs.vertexbuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, model_specs.uvbuffer);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, model_specs.normalbuffer);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
}

void Enable_Instance_Buffers(instanced_buffer_specs instance_specs)
{
	// 4th Attribute Buffer: Positions
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, instance_specs.ship_transform_buffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 5th Attribute Buffer: Rotations
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, instance_specs.ship_rotation_buffer);
	glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
}

void Disable_Vertex_Attrib_Arrays()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Disable_Instance_Vertex_Attrib_Array()
{
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void Draw_Element_Array(model_buffer_specs model_specs)
{
	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_specs.elementbuffer);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, model_specs.num_vertices_to_draw, GL_UNSIGNED_SHORT, (void*)0);
}

void Draw_Instance_Array(model_buffer_specs model_specs, int num_objects_to_draw)
{
	glVertexAttribDivisor(0, 0); // particles vertices 
	glVertexAttribDivisor(1, 0); // particles uvs 
	glVertexAttribDivisor(2, 0); // particles normals
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, model_specs.num_vertices_to_draw, num_objects_to_draw);
}

void Draw_Object(GLFWwindow* window, GLuint shader_program, model_buffer_specs model_specs, vec3 lightPos, mat4 scale_matrix, mat4 transform_matrix, mat4 rotation_matrix, vec3 Base_Color)
{
	// Use our shader
	glUseProgram(shader_program);

	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = transform_matrix * rotation_matrix * scale_matrix;
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(ColorID, Base_Color.x, Base_Color.y, Base_Color.z);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	Enable_Model_Buffers(model_specs);

	Draw_Element_Array(model_specs);

	Disable_Vertex_Attrib_Arrays();
}

void Draw_Instanced_Object(GLFWwindow* window, GLuint shader_program, vec3 lightPos, model_buffer_specs model_specs, instanced_buffer_specs instance_specs, int num_items_to_render, vec3 Base_Color, vec3* swarm_transforms, vec3* swarm_rotations)
{
	// Use our shader
	glUseProgram(shader_program);

	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(ColorID, Base_Color.x, Base_Color.y, Base_Color.z);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	Enable_Model_Buffers(model_specs);
	Enable_Instance_Buffers(instance_specs);
	stream_instance_data_to_buffers(instance_specs, num_items_to_render, swarm_transforms, swarm_rotations);
	
	Draw_Instance_Array(model_specs, num_items_to_render);

	Disable_Vertex_Attrib_Arrays();
	Disable_Instance_Vertex_Attrib_Array();
}

void Cleanup_Object(model_buffer_specs* model_specs)
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &model_specs->vertexbuffer);
	glDeleteBuffers(1, &model_specs->uvbuffer);
	glDeleteBuffers(1, &model_specs->normalbuffer);
	glDeleteBuffers(1, &model_specs->elementbuffer);
	glDeleteVertexArrays(1, &model_specs->VertexArrayID);
	glDeleteTextures(1, &Texture);
}