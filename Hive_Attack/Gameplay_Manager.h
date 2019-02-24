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

#include<Render_Manager.h>
#include<Object_Static.h>

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

	void Init(Service_Locator* service_locator);

	void update_scene_graph();

	void cleanup_scene_graph();

	void draw_scene_graph(GLFWwindow* window);

	void Draw_Hive(GLFWwindow * window, int shader_program, glm::vec3 lightPos, Hive_Object * hive_pointer);

	void Draw_Hive_Ship_Array(GLFWwindow* window, int shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer);

	void Draw_Projectiles(GLFWwindow * window, int shader_program);

	void Handle_Mouse_Click(double x_pos, double y_pos);

	void load_base_octo();

	void load_base_ship();

	void load_base_projectile();

	void Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array);

	void Remove_Hive_Ship_Array_From_Manifest(Hive_Ship_Array * ship_array);

	void Create_New_Projectile(vec3 start_position, vec3 end_position);

	void Update_Projectile_Model_Matrices();

	void Update_Projectile_Color_Matrices();

	//bool Set_Hive_Engagement_Target(Hive_Object * base_hive, Hive_Object* hive_engagement_target);

	void Set_Hive_Swarm_Engagement_Target(Hive_Ship_Array* swarm, Hive_Object* hive_engagement_target);

private:
	Service_Locator * service_locator;

	vector<Hive_Object*> Hive_Object_Array;

	Hive_Ship_Array_Manifest hive_ship_array_manifest;

	vector<Projectile*> projectile_array;

	glm::vec3 lightPos = glm::vec3(0, 10, 0);

	model_buffer_specs hive_pod;
	model_buffer_specs simple_ship;
	model_buffer_specs simple_projectile;

	mat4 projectile_model_matrices[1000];
	vec3 projectile_color_matrices[1000];
};



