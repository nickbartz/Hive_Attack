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

Hive_Object Hive_Object_Array[2];
int num_hive_objects = 2;

instanced_buffer_specs ship_instance_specs;

Hive_Ship_Array_Manifest ship_manifest;

void init_scene_graph()
{
	Hive_Object_Array[0].Init_Hive_Object({0.75,0.75,0.0}, 0.02);
	for (int i = 0; i < 50; i++) Hive_Object_Array[0].extrude_new_octo();
	Hive_Object_Array[0].update_translation_matrix({ -20.0,0.0,0.1 });
	Hive_Object_Array[0].update_model_vertices();
	Hive_Object_Array[0].load_buffer_return_specs();
	Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array[0].hive_ship_array);

	Hive_Object_Array[1].Init_Hive_Object({ 0.75,0.0,0.0 },0.005);
	for (int i = 0; i < 50; i++) Hive_Object_Array[1].extrude_new_octo();
	Hive_Object_Array[1].update_translation_matrix({ 20.0,0.0,0.1 });
	Hive_Object_Array[1].update_model_vertices();
	Hive_Object_Array[1].load_buffer_return_specs();
	Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array[1].hive_ship_array);

	ship_instance_specs = load_instance_buffers(MAX_SHIPS_PER_HIVE);
}

void cleanup_scene_graph()
{
	for (int i = 0; i < num_hive_objects; i++)
	{
		Cleanup_Object(&Hive_Object_Array[i].loaded_specs);
	}
}

void update_scene_graph()
{
	for (int i = 0; i < ship_manifest.Array_End(); i++)
	{
		check_for_swarm_engagement_target(ship_manifest.Return_Ship_Array(i));

		for (int p = 0; p < ship_manifest.Return_Ship_Array(i)->array_end(); p++)
		{
			Ship_Object* ship = ship_manifest.Return_Ship_Array(i)->return_ship_in_array(p);

			if (ship != NULL)
			{
				check_ship_engagement_target(ship, ship_manifest.Return_Ship_Array(i)->return_engaged_ship_array());

				process_ship_damage(ship);

				ship->move();

				if (ship->get_health() <= 0.0) ship_manifest.Return_Ship_Array(i)->remove_ship_from_array(ship);
			}
		}

	}
}

void check_if_swarm_is_dead(Hive_Ship_Array* ship_array)
{

}

void check_for_swarm_engagement_target(Hive_Ship_Array* ship_array)
{
	if (ship_array->is_engaged() == false)
	{
		for (int i = 0; i < ship_manifest.Array_End(); i++)
		{
			if (ship_manifest.Return_Ship_Array(i)->return_uniq_id() != ship_array->return_uniq_id())
			{
				ship_array->set_engagement_target(ship_manifest.Return_Ship_Array(i));
				ship_array->set_engaged(true);
				if (ship_manifest.Return_Ship_Array(i)->is_engaged() == false) ship_manifest.Return_Ship_Array(i)->set_engagement_target(ship_array);
				break;
			}
		}
	}
}

void check_ship_engagement_target(Ship_Object* ship, Hive_Ship_Array* swarm_two)
{
	if (ship->is_engaged() == false)
	{
		Ship_Object* target = swarm_two->return_ship_in_array(rand() % swarm_two->array_end());
		if (target != NULL)
		{
			ship->set_engagement_target(target);
			if (target->is_engaged() == false)
			{
				target->set_engagement_target(ship);
			}
		}

	}
	else if (ship->return_current_engagement_target() == NULL || ship->return_current_engagement_target()->get_health() <= 0.0)
	{
		ship->remove_engagement_target();
		ship->set_engaged(false);
		ship->set_new_orbit(ship->return_home_orbit());
	}
}

void process_ship_damage(Ship_Object* ship)
{
	ship->double_check_engaged_ships();

	ship->calculate_damage_from_engaging_ships();

	ship->Reduce_Ship_Health(ship->calculate_damage_from_engaging_ships());
}

void draw_scene_graph(GLFWwindow* window, GLuint shader_program, GLuint instance_render_shader, glm::vec3 lightPos)
{
	for (int i = 0; i < num_hive_objects; i++)
	{
		Draw_Object(window, shader_program, Hive_Object_Array[i].loaded_specs, lightPos, Hive_Object_Array[i].ScaleMatrix, Hive_Object_Array[i].TranslationMatrix, Hive_Object_Array[i].RotationMatrix, Hive_Object_Array[i].return_hive_color());
	}
	for (int i = 0; i < ship_manifest.Array_End(); i++)
	{
		Draw_Hive_Ship_Array(window, instance_render_shader, lightPos, ship_manifest.Return_Ship_Array(i));
	}
}

void Draw_Hive_Ship_Array(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer)
{
	model_buffer_specs* ship_model_type = Return_Ship_Model_Buffer_Specs(ship_array_pointer->return_ship_model_type());
	vec3 ship_array_color = ship_array_pointer->return_ship_array_color();

	//for (int i = ship_array_pointer->array_begin(); i < ship_array_pointer->array_end(); i++)
	//{
	//	Ship_Object* ship = ship_array_pointer->return_ship_in_array(i);
	//	if (ship != NULL && ship->is_active() == true)
	//	{
	//		Draw_Object(window, shader_program, *ship_model_type, lightPos, ship->ScaleMatrix, ship->Transform_Matrix, ship->RotationMatrix, ship_array_color);
	//	}
	//}

	vec3* ship_transforms = ship_array_pointer->return_ships_transforms();
	vec3* ship_rotations = ship_array_pointer->return_ships_rotations();

	Draw_Instanced_Object(window, shader_program, lightPos, *ship_model_type, ship_instance_specs, ship_array_pointer->return_num_ships_in_swarm(), ship_array_color, ship_transforms, ship_rotations);
}

void Handle_Mouse_Click(double x_pos, double y_pos)
{
	for (int i = 0; i < Hive_Object_Array[0].num_current_cells; i++)
	{
		Hive_Object_Array[0].cell_array[i].hive_ship_pointer->set_new_orbit({x_pos, 0.0, y_pos});
	}
}

model_buffer_specs* Return_Ship_Model_Buffer_Specs(int ship_model)
{
	// Ultimately want to load the specs from some centralized model warehouse
	return &Hive_Object_Array[0].loaded_ship_specs;
}


// Object Specific Functions // 

void Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array)
{
	ship_manifest.add_hive_ship_array_to_manifest(ship_array);
}

void Remove_Hive_Ship_Array_From_Manifest(Hive_Ship_Array* ship_array)
{
	ship_manifest.remove_hive_ship_array_from_manifest(ship_array);
}

