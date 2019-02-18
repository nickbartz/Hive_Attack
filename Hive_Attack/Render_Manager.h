#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>;
#include<iostream>

using namespace std;
using namespace glm;

class Service_Locator;

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


enum Shader_Type
{
	SHADER_STANDARD,
	SHADER_INSTANCE,
	SHADER_TEXT,
	SHADER_BILLBOARD
};

class Render_Manager
{
public:
	Render_Manager();

	int Init(Service_Locator* service_locator);

	model_buffer_specs create_object_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices);

	void Draw_Object(GLFWwindow* window, int shader_type, model_buffer_specs model_specs, glm::vec3 lightPos, glm::mat4 model_matrix, glm::vec3 Base_Color);

	void Draw_Instanced_Object(GLFWwindow * window, int shader_type, glm::vec3 lightPos, model_buffer_specs model_specs, int num_items_to_render, vec3 Base_Color, mat4* swarm_model_matrices);

	void printText2D(const char * text, int x, int y, int size);

	void computeMatricesFromInputs(GLFWwindow* window);

	void handle_mouse_input(GLFWwindow* window, int button, int action, int mods);

	~Render_Manager();
private:
	Service_Locator * service_locator;

	GLuint shaders[4];

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
	GLuint ProjectionMatrixID_Instance;
	GLuint TextureID_Instance;
	GLuint LightID_Instance;
	GLuint ColorID_Instance;

	unsigned int Text2DTextureID;
	unsigned int Text2DVertexBufferID;
	unsigned int Text2DUVBufferID;
	unsigned int Text2DUniformID;

	float speed = 40.0f; // 3 units / second
	float mouseSpeed = 0.0005f;

	// Matrices
	float FoV = 45.0f;
	glm::mat4 projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
	glm::mat4 View;

	glm::vec3 position = glm::vec3(0, 25, 40);
	glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 right_vector = { 0.0f, 0.0f, 0.0f };
	glm::vec3 up = { 0.0f, 0.0f, 0.0f };

	// horizontal angle : toward -Z
	float horizontalAngle = 3.14158;
	// vertical angle : 0, look at the horizon
	float verticalAngle = -0.6f;

	double currentTime;
	float deltaTime;

	GLuint Texture;

	void Cleanup_Object(model_buffer_specs* object_specs);

	void assign_uniform_pointers();

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	void initText2D(const char * texturePath);

	glm::mat4 getProjectionMatrix();

	glm::mat4 getViewMatrix();
};



