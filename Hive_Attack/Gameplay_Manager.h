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
#include<iostream>

class Projectile;
class Hive_Ship_Array;
class Hive_Object;

class Gameplay_Manager
{
public:
	Gameplay_Manager();

	void Init(Service_Locator* service_locator);

	void update_scene_graph();



	void Create_New_Projectile(vec3 start_position, vec3 end_position);

	void Create_New_Hive(vec3 location, vec3 color, float damage, model_buffer_specs* pod_model, model_buffer_specs* ship_model, int num_octos);

	void cleanup_scene_graph();

	void choose_swarm_targets(Hive_Object * hive_object);

	void draw_scene_graph(GLFWwindow* window);

	void Draw_Hive(GLFWwindow * window, int shader_program, glm::vec3 lightPos, Hive_Object * hive_pointer);

	void Draw_Hive_Ship_Array(GLFWwindow* window, int shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer);

	void Draw_Projectiles(GLFWwindow * window, int shader_program);

	void Handle_Mouse_Click(double x_pos, double y_pos);

	bool Hive_Respond_To_Nearby_Threats(Hive_Object* hive_object);

	bool Hive_Respond_To_Nearby_Conquest_Opportunities(Hive_Object * hive_object);

	bool Hive_Respond_To_Available_Plundering_Opportunities(Hive_Object * hive_object);

	void load_base_octo();

	void load_base_ship();

	void load_base_projectile();

	Hive_Ship_Array* Return_Nearest_Swarm(Hive_Object* base_object);

	void Update_Projectile_Model_Matrices();

	void Update_Projectile_Color_Matrices();

private:
	Service_Locator * service_locator;

	bool first_run = true;

	vector<Hive_Object*> Hive_Object_Array;

	vector<Projectile*> projectile_array;

	glm::vec3 lightPos = glm::vec3(0, 10, 0);

	model_buffer_specs hive_pod;
	model_buffer_specs simple_ship;
	model_buffer_specs simple_projectile;

	mat4 projectile_model_matrices[1000];
	vec3 projectile_color_matrices[1000];
};



