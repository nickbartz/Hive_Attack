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
//#include<Scene_Objects.h>

#include<iostream>

class Hive_Ship_Array;
class Ship_Object;

const int MAX_SHIPS_PER_HIVE = 1000;

struct Grid_Coord
{
	int grid_x = 0, grid_y = 0, grid_z = 0;

	void print()
	{
		std::cout << grid_x << ":" << grid_y << ":" << grid_z;
	}

};

inline bool operator<(const Grid_Coord& p1, const Grid_Coord& p2) {
	if (p1.grid_x != p2.grid_x) {
		return p1.grid_x < p2.grid_x;
	}
	else if (p1.grid_y != p2.grid_y) {
		return p1.grid_y < p2.grid_y;
	}
	else
	{
		return p1.grid_z < p2.grid_z;
	}
}

inline Grid_Coord operator+(const Grid_Coord& p1, const Grid_Coord& p2)
{
	Grid_Coord new_coord;

	new_coord.grid_x = p1.grid_x + p2.grid_x;
	new_coord.grid_y = p1.grid_y + p2.grid_y;
	new_coord.grid_z = p1.grid_z + p2.grid_z;

	return new_coord;
}

void init_scene_graph();

void cleanup_scene_graph();

void update_scene_graph();

void process_ship_damage(Ship_Object * ship_array);

void draw_scene_graph(GLFWwindow* window, GLuint shader_program, GLuint instance_render_shader, glm::vec3 lightPos);

void Handle_Mouse_Click(double x_pos, double y_pos);

model_buffer_specs* Return_Ship_Model_Buffer_Specs(int ship_model);

void Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array);

void Draw_Hive_Ship_Array(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer);

void check_for_swarm_engagement_target(Hive_Ship_Array * ship_array);

void check_ship_engagement_target(Ship_Object * ship, Hive_Ship_Array * swarm_two);
