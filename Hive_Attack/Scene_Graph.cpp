#include<Scene_Graph.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include<vector>
#include<map>
#include<iostream>

#include<2DText.h>
#include<controls.h>
#include<loadShader.h>
#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Draw_Object.h>
#include<Scene_Objects.h>

using namespace std;
using namespace glm;

Gameplay_Manager::Gameplay_Manager()
{

}

void Gameplay_Manager::init_scene_graph()
{
	Hive_Object_Array.push_back(new Hive_Object);
	Hive_Object_Array[0]->Init_Hive_Object({ -20.0,0.0,0.0 } ,{0.75,0.75,0.0}, 0.02);
	for (int i = 0; i < 50; i++) Hive_Object_Array[0]->extrude_new_octo();
	Hive_Object_Array[0]->load_buffer_return_specs();
	Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array[0]->hive_ship_array);

	Hive_Object_Array.push_back(new Hive_Object);
	Hive_Object_Array[1]->Init_Hive_Object({ 20.0,0.0,0.0 }, { 0.75,0.0,0.0 }, 0.02);
	for (int i = 0; i < 50; i++) Hive_Object_Array[1]->extrude_new_octo();
	Hive_Object_Array[1]->load_buffer_return_specs();
	Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array[1]->hive_ship_array);

	// Set the Hive's Initial Target
	Set_Hive_Engagement_Target(Hive_Object_Array[0], Hive_Object_Array[1]);
}

void Gameplay_Manager::cleanup_scene_graph()
{
	for (int i = 0; i < Hive_Object_Array.size(); i++)
	{
		Cleanup_Object(&Hive_Object_Array[i]->loaded_specs);
	}
}

void Gameplay_Manager::update_scene_graph()
{
	for (int i = 0; i < Hive_Object_Array.size(); i++)
	{
		Hive_Object* hive_object = Hive_Object_Array[i];

		if (hive_object != NULL)
		{
			// Erase the hive if the hive has no more pods
			if (hive_object->return_num_current_pods() <= 0) Hive_Object_Array.erase(Hive_Object_Array.begin() + i);
			else
			{
				// If the hive is engaged, check to see if the hive it is engaged with still has a fleet, if not, go send the ships to plunder
				if (hive_object->is_engaged() && hive_object->check_engagement_target_fleet_destroyed())
				{
					hive_object->return_hive_ship_array()->send_ships_to_collect_enemy_hive_pods(hive_object->return_hive_engagement_target());
				}

				// Update the hives ship arrays
				Hive_Object_Array[i]->update_ship_arrays();

				// Update the hive's pod arrays
				Hive_Object_Array[i]->update_pod_array();
			}
		}

		if (rand() % 50 == 0) Hive_Object_Array[i]->extrude_new_octo();
	}
}

bool Gameplay_Manager::Set_Hive_Engagement_Target(Hive_Object* hive, Hive_Object* hive_engagement_target)
{
	hive->set_hive_engagement_target(hive_engagement_target);
	if (hive_engagement_target->return_hive_engagement_target() == NULL) hive_engagement_target->set_hive_engagement_target(hive);

	return true;
}

void Gameplay_Manager::draw_scene_graph(GLFWwindow* window, GLuint shader_program, GLuint instance_render_shader)
{

	for (int i = 0; i < Hive_Object_Array.size(); i++)
	{
		Draw_Hive(window, instance_render_shader, lightPos, Hive_Object_Array[i]);
		Draw_Hive_Ship_Array(window, instance_render_shader, lightPos, Hive_Object_Array[i]->return_hive_ship_array());
	}
}

void Gameplay_Manager::Draw_Hive(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos, Hive_Object* hive_pointer)
{
	model_buffer_specs* hive_model_type = hive_pointer->return_loaded_hive_specs();
	vec3 hive_pod_array_color = hive_pointer->Hive_Color;

	mat4* hive_pod_model_matrices = hive_pointer->return_hive_pods_model_matrices();

	Draw_Instanced_Object(window, shader_program, lightPos, *hive_model_type, hive_pointer->return_num_current_pods(), hive_pod_array_color, hive_pod_model_matrices);
}

void Gameplay_Manager::Draw_Hive_Ship_Array(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer)
{
	model_buffer_specs* ship_model_type = ship_array_pointer->return_loaded_ship_specs();
	vec3 ship_array_color = ship_array_pointer->return_ship_array_color();

	mat4* ship_model_matrices = ship_array_pointer->return_ships_model_matrices();

	Draw_Instanced_Object(window, shader_program, lightPos, *ship_model_type, ship_array_pointer->return_num_ships_in_swarm(), ship_array_color, ship_model_matrices);
}

void Gameplay_Manager::Handle_Mouse_Click(double x_pos, double y_pos)
{
	for (int i = 0; i < Hive_Object_Array[0]->num_current_hive_pods; i++)
	{
		Hive_Object_Array[0]->Hive_Pod_Array[i].hive_ship_pointer->set_new_orbit({x_pos, 0.0, y_pos});
	}
}

model_buffer_specs* Gameplay_Manager::Return_Hive_Pod_Model_Buffer_Specs(int hive_pod_model)
{
	return &Hive_Object_Array[0]->loaded_specs;
}

void Gameplay_Manager::Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array)
{
	hive_ship_array_manifest.add_hive_ship_array_to_manifest(ship_array);
}

void Gameplay_Manager::Remove_Hive_Ship_Array_From_Manifest(Hive_Ship_Array* ship_array)
{
	hive_ship_array_manifest.remove_hive_ship_array_from_manifest(ship_array);
}