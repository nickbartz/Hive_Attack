#include<Render_Manager.h>

#include<fstream>
#include<sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include<texture.hpp>
#include<Global_Render_Variables.h>
#include<Gameplay_Manager.h>
#include<Service_Locator.h>

using namespace std;
using namespace glm;

Render_Manager::Render_Manager()
{

}

int Render_Manager::Init(Service_Locator* _service_locator)
{
	service_locator = _service_locator;

	// Create and compile our GLSL program from the shaders
	shaders[SHADER_STANDARD] = LoadShaders("StandardShading.vertexshader", "StandardTransparentShading.fragmentshader");
	shaders[SHADER_INSTANCE] = LoadShaders("Instance_Render.vertexshader", "Instance_Render.fragmentshader");
	shaders[SHADER_TEXT] = LoadShaders("TextVertexShader.vertexshader", "TextFragmentShader.fragmentshader");
	shaders[SHADER_BILLBOARD] = LoadShaders("Billboard_Render.vertexshader", "Billboard_Render.fragmentshader");

	assign_uniform_pointers();
	
	initText2D("Holstein.DDS");
	
	return 0;
}

void Render_Manager::assign_uniform_pointers()
{
	MatrixID = glGetUniformLocation(shaders[SHADER_STANDARD], "MVP");
	ViewMatrixID = glGetUniformLocation(shaders[SHADER_STANDARD], "V");
	ModelMatrixID = glGetUniformLocation(shaders[SHADER_STANDARD], "M");
	TextureID = glGetUniformLocation(shaders[SHADER_STANDARD], "myTextureSampler");
	LightID = glGetUniformLocation(shaders[SHADER_STANDARD], "LightPosition_worldspace");
	ColorID = glGetUniformLocation(shaders[SHADER_STANDARD], "BaseColor");

	ViewMatrixID_Instance = glGetUniformLocation(shaders[SHADER_INSTANCE], "V");
	ProjectionMatrixID_Instance = glGetUniformLocation(shaders[SHADER_INSTANCE], "P");
	TextureID_Instance = glGetUniformLocation(shaders[SHADER_INSTANCE], "myTextureSampler");
	LightID_Instance = glGetUniformLocation(shaders[SHADER_INSTANCE], "LightPosition_worldspace");
	ColorID_Instance = glGetUniformLocation(shaders[SHADER_INSTANCE], "BaseColor");

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation(shaders[SHADER_TEXT], "text_texture");
}

model_buffer_specs Render_Manager::create_object_buffers(vector<glm::vec3> &indexed_vertices, vector<glm::vec2> indexed_uvs, vector<glm::vec3> indexed_normals, vector<unsigned short> indices)
{
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Generates a Model_Matrix_Buffer for Instancing
	GLuint model_matrix_instance_buffer;
	glGenBuffers(1, &model_matrix_instance_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, model_matrix_instance_buffer);
	for (unsigned int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
		glVertexAttribDivisor(3 + i, 1);
	}

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// Bind our texture in Texture Unit 0
	Texture = loadBMP_custom("Cell_Texture.bmp");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	model_buffer_specs new_model;

	new_model.VertexArrayID = VertexArrayID;
	new_model.vertexbuffer = vertexbuffer;
	new_model.uvbuffer = uvbuffer;
	new_model.normalbuffer = normalbuffer;
	new_model.elementbuffer = elementbuffer;
	new_model.num_indices = indices.size();
	new_model.model_matrix_instance_buffer = model_matrix_instance_buffer;

	return new_model;
}

void Render_Manager::Draw_Object(GLFWwindow* window, int shader_program, model_buffer_specs model_specs, vec3 lightPos, mat4 model_matrix, vec3 Base_Color)
{
	// Use our shader
	glUseProgram(shaders[shader_program]);

	glBindVertexArray(model_specs.VertexArrayID);

	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = model_matrix;
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

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, model_specs.num_indices, GL_UNSIGNED_SHORT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Render_Manager::Draw_Instanced_Object(GLFWwindow* window, int shader_program, vec3 lightPos, model_buffer_specs model_specs, int num_items_to_render, vec3 Base_Color, mat4* swarm_model_matrices)
{
	// Use our shader
	glUseProgram(shaders[shader_program]);

	glBindVertexArray(model_specs.VertexArrayID);

	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, model_specs.model_matrix_instance_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * num_items_to_render, swarm_model_matrices, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(ViewMatrixID_Instance, 1, GL_FALSE, &ViewMatrix[0][0]);
	glUniformMatrix4fv(ProjectionMatrixID_Instance, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glUniform3f(LightID_Instance, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(ColorID_Instance, Base_Color.x, Base_Color.y, Base_Color.z);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID_Instance, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glDrawElementsInstanced(GL_TRIANGLES, model_specs.num_indices, GL_UNSIGNED_SHORT, 0 , num_items_to_render);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
}

void Render_Manager::Cleanup_Object(model_buffer_specs* model_specs)
{
	// Cleanup VBO and shader
	glDeleteBuffers(1, &model_specs->vertexbuffer);
	glDeleteBuffers(1, &model_specs->uvbuffer);
	glDeleteBuffers(1, &model_specs->normalbuffer);
	glDeleteBuffers(1, &model_specs->elementbuffer);
	glDeleteVertexArrays(1, &model_specs->VertexArrayID);
	glDeleteTextures(1, &Texture);
}

GLuint Render_Manager::LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);

	if (VertexShaderStream.is_open())
	{
		stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		printf("could not open %s \n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);

	if (FragmentShaderStream.is_open())
	{
		stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	else
	{
		printf("could not open %s \n", fragment_file_path);
		getchar();
		return 0;
	}

	GLint Result = GL_FALSE;

	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check FragmentShader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;


}

void Render_Manager::initText2D(const char * texturePath) {

	// Initialize texture
	Text2DTextureID = loadDDS(texturePath);

	// Initialize VBO
	glGenBuffers(1, &Text2DVertexBufferID);
	glGenBuffers(1, &Text2DUVBufferID);

}

void Render_Manager::printText2D(const char * text, int x, int y, int size) {

	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i<length; i++) {

		glm::vec2 vertex_up_left = glm::vec2(x + i * size, y + size);
		glm::vec2 vertex_up_right = glm::vec2(x + i * size + size, y + size);
		glm::vec2 vertex_down_right = glm::vec2(x + i * size + size, y);
		glm::vec2 vertex_down_left = glm::vec2(x + i * size, y);

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(shaders[SHADER_TEXT]);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(Text2DUniformID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

glm::mat4 Render_Manager::getProjectionMatrix()
{
	return glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
}

glm::mat4 Render_Manager::getViewMatrix()
{
	return glm::lookAt(position, direction + position, up);
}

void Render_Manager::computeMatricesFromInputs(GLFWwindow* window)
{
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	//// Get mouse position
	//double xpos, ypos;
	//glfwGetCursorPos(window, &xpos, &ypos);

	//// Reset mouse position for next frame
	//glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	//horizontalAngle += mouseSpeed * float(WINDOW_WIDTH / 2 - xpos);
	//verticalAngle += mouseSpeed * float(WINDOW_HEIGHT / 2 - ypos);

	// Position Vector
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position.x += direction.x * deltaTime * speed;
		position.z += direction.z * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position.x -= direction.x * deltaTime * speed;
		position.z -= direction.z * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right_vector * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right_vector * deltaTime * speed;
	}

	// Move North 
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}

	// Move South 
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}

	// Direction Vector
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle));

	// Right Vector
	right_vector = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f));

	// Up Vector
	up = glm::cross(right_vector, direction);

	lastTime = currentTime;
}

void Render_Manager::handle_mouse_input(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		float x = (2.0f * xpos) / WINDOW_WIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / WINDOW_HEIGHT;
		float z = 1.0f;

		glm::vec3 ray_nds = glm::vec3(x, y, z);

		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

		glm::vec4 ray_eye = glm::inverse(getProjectionMatrix()) * ray_clip;

		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

		glm::vec3 ray_wor = (inverse(getViewMatrix()) * ray_eye);
		ray_wor = glm::normalize(ray_wor);

		float scalar = -position.y / ray_wor.y;

		glm::vec3 new_point = position + ray_wor * scalar;

		cout << new_point.x << ", " << new_point.y << ", " << new_point.z << endl;

		service_locator->return_gameplay_manager()->Handle_Mouse_Click(new_point.x, new_point.z);
	}
}

Render_Manager::~Render_Manager()
{
	// Delete buffers
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Text2DTextureID);

	glDeleteProgram(shaders[SHADER_STANDARD]);
	glDeleteProgram(shaders[SHADER_INSTANCE]);
	glDeleteProgram(shaders[SHADER_TEXT]);
}