#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <Global_Render_Variables.h>

#include<Draw_Object.h>
#include<Scene_Objects.h>

#include<iostream>

inline Grid_Coord operator+(const Grid_Coord& p1, const Grid_Coord& p2)
{
	Grid_Coord new_coord;

	new_coord.grid_x = p1.grid_x + p2.grid_x;
	new_coord.grid_y = p1.grid_y + p2.grid_y;
	new_coord.grid_z = p1.grid_z + p2.grid_z;

	return new_coord;
}

class Gameplay_Manager
{
public:
	Gameplay_Manager();

	void init_scene_graph();

	void update_scene_graph();

	void cleanup_scene_graph();

	void draw_scene_graph(GLFWwindow* window, GLuint shader_program, GLuint instance_render_shader);

	void Draw_Hive(GLFWwindow * window, GLuint shader_program, glm::vec3 lightPos, Hive_Object * hive_pointer);

	void Handle_Mouse_Click(double x_pos, double y_pos);

	model_buffer_specs * Return_Hive_Pod_Model_Buffer_Specs(int hive_pod_model);

	void Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array);

	void Remove_Hive_Ship_Array_From_Manifest(Hive_Ship_Array * ship_array);

	void Draw_Hive_Ship_Array(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer);

	bool Set_Hive_Engagement_Target(Hive_Object * base_hive, Hive_Object* hive_engagement_target);

private:
	vector<Hive_Object*> Hive_Object_Array;

	Hive_Ship_Array_Manifest hive_ship_array_manifest;

	glm::vec3 lightPos = glm::vec3(0, 10, 10);

};



