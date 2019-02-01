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

GLuint Texture;
GLuint TextureID;
GLuint LightID;

buffer_specs load_object_to_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices)
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

void assign_uniform_pointers(GLuint programID)
{
	glUseProgram(programID);
	
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

}

void Draw_Object(GLFWwindow* window, GLuint shader_program, buffer_specs model_specs, glm::vec3 lightPos, glm::mat4 scale_matrix, glm::mat4 transform_matrix, glm::mat4 rotation_matrix)
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

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

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

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_specs.elementbuffer);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES,model_specs.num_vertices_to_draw,GL_UNSIGNED_SHORT,(void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Cleanup_Object(buffer_specs* model_specs)
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &model_specs->vertexbuffer);
	glDeleteBuffers(1, &model_specs->uvbuffer);
	glDeleteBuffers(1, &model_specs->normalbuffer);
	glDeleteBuffers(1, &model_specs->elementbuffer);
	glDeleteVertexArrays(1, &model_specs->VertexArrayID);
	glDeleteTextures(1, &Texture);
}