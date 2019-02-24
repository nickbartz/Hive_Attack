#include<Gameplay_Manager.h>
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

#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Service_Locator.h>
#include<Memory_Manager.h>

using namespace std;
using namespace glm;

Gameplay_Manager::Gameplay_Manager()
{

}

void Gameplay_Manager::Init(Service_Locator* _service_locator)
{
	service_locator = _service_locator;

	load_base_octo();
	load_base_ship();
	load_base_projectile();

	Hive_Object_Array.push_back(new Hive_Object);
	Hive_Object_Array[0]->Init_Hive_Object(service_locator, { 0.0,0.0,0.0 }, { 0.75,0.75,0.0 }, 0.02, &hive_pod, &simple_ship);
	for (int i = 0; i < 10; i++) Hive_Object_Array[0]->extrude_new_octo({ 0.75,0.75,0.0 });
	//Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array[0]->hive_ship_array);

	for (int i = 1; i < 10; i++)
	{
		int rand_x = rand() % 2 == 0 ? 1 : -1;
		int rand_z = rand() % 2 == 0 ? 1 : -1;

		float rgb_r = rand() % 100 / 100.0;
		float rgb_g = rand() % 100 / 100.0;
		float rgb_b = rand() % 100 / 100.0;

		Hive_Object_Array.push_back(new Hive_Object);

		Hive_Object_Array.back()->Init_Hive_Object(service_locator, { 10.0*rand_x*i,0.0,10.0*rand_z*i }, { rgb_r,rgb_g,rgb_b }, 0.02, &hive_pod, &simple_ship);
		for (int p = 0; p < 3 + i; p++) Hive_Object_Array.back()->extrude_new_octo({ rgb_r,rgb_g,rgb_b });
		//Add_Hive_Ship_Array_To_Manifest(&Hive_Object_Array.back()->hive_ship_array);
	}
}

void Gameplay_Manager::Add_Hive_Ship_Array_To_Manifest(Hive_Ship_Array* ship_array)
{
	hive_ship_array_manifest.add_hive_ship_array_to_manifest(ship_array);
}

void Gameplay_Manager::cleanup_scene_graph()
{
	for (int i = 0; i < Hive_Object_Array.size(); i++)
	{
		//Cleanup_Object(&Hive_Object_Array[i]->loaded_specs);
	}
}

void Gameplay_Manager::draw_scene_graph(GLFWwindow* window)
{
	for (int i = 0; i < Hive_Object_Array.size(); i++)
	{
		Draw_Hive(window, SHADER_INSTANCE, lightPos, Hive_Object_Array[i]);

		for (int p = 0; p < Hive_Object_Array[i]->return_num_ship_arrays(); p++)
		{
			Draw_Hive_Ship_Array(window, SHADER_INSTANCE, lightPos, Hive_Object_Array[i]->return_ship_array_by_num(p));
		}

	}

	Draw_Projectiles(window, SHADER_INSTANCE);
}

void Gameplay_Manager::Draw_Hive(GLFWwindow* window, int shader_program, glm::vec3 lightPos, Hive_Object* hive_pointer)
{
	model_buffer_specs* hive_model_type = hive_pointer->return_loaded_hive_pod_model();

	vec3 hive_pod_array_color = hive_pointer->Hive_Color;

	mat4* hive_pod_model_matrices = hive_pointer->return_hive_pods_model_matrices();

	vec3* hive_pod_color_matrices = hive_pointer->return_hive_pods_color_matrices();

	service_locator->return_render_manager()->Draw_Instanced_Object(window, shader_program, lightPos, *hive_model_type, hive_pointer->return_num_current_pods(), hive_pod_array_color, hive_pod_model_matrices, hive_pod_color_matrices);
}

void Gameplay_Manager::Draw_Hive_Ship_Array(GLFWwindow* window, int shader_program, glm::vec3 lightPos, Hive_Ship_Array* ship_array_pointer)
{
	model_buffer_specs* ship_model_type = ship_array_pointer->return_loaded_ship_specs();
	vec3 ship_array_color = ship_array_pointer->return_ship_array_color();

	mat4* ship_model_matrices = ship_array_pointer->return_model_matrix_array();

	vec3* hive_pod_color_matrices = ship_array_pointer->return_ships_color_matrices();

	service_locator->return_render_manager()->Draw_Instanced_Object(window, shader_program, lightPos, *ship_model_type, ship_array_pointer->return_num_ships_in_swarm(), ship_array_color, ship_model_matrices, hive_pod_color_matrices);
}

void Gameplay_Manager::Draw_Projectiles(GLFWwindow* window, int shader_program)
{
	vec3 ship_array_color = { 1.0f,1.0f,1.0f };

	if (projectile_array.size() > 0)
	{
		service_locator->return_render_manager()->Draw_Instanced_Object(window, shader_program, lightPos, simple_projectile, projectile_array.size(), ship_array_color, projectile_model_matrices, projectile_color_matrices);
	}

}

void Gameplay_Manager::Handle_Mouse_Click(double x_pos, double y_pos)
{
	for (int i = 1; i < Hive_Object_Array.size(); i++)
	{
		vec3 hive_translation_vector = Hive_Object_Array[i]->return_hive_translation_vector();

		if (abs(hive_translation_vector.x - x_pos) < 3.0 && abs(hive_translation_vector.z - y_pos) < 3.0)
		{
			Set_Hive_Swarm_Engagement_Target(Hive_Object_Array[0]->return_ship_array_by_num(0), Hive_Object_Array[i]);
			Hive_Object_Array[i]->add_attacking_swarm(Hive_Object_Array[0]->return_ship_array_by_num(0));
			return;
		}
	}
}

void Gameplay_Manager::load_base_octo()
{
	// Temp Containers for the Octohedron's Details
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	Octohedron_Model octohedron_base;

	octohedron_base.init_base_octohedron();
	octohedron_base.add_triangle_array_components(vertices, uvs, normals);
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
	
	hive_pod = service_locator->return_render_manager()->create_object_buffers(indexed_vertices,indexed_uvs,indexed_normals,indices);
}

void Gameplay_Manager::load_base_ship()
{
	// Vertices for the Ship Model
	std::vector<glm::vec3> ship_vertices;
	std::vector<glm::vec2> ship_uvs;
	std::vector<glm::vec3> ship_normals;
	std::vector<glm::vec3> ship_indexed_vertices;
	std::vector<glm::vec2> ship_indexed_uvs;
	std::vector<glm::vec3> ship_indexed_normals;
	std::vector<unsigned short> ship_indices;

	loadOBJ_no_uvs("Basic_Ship.obj", ship_vertices, ship_uvs, ship_normals);
	indexVBO(ship_vertices, ship_uvs, ship_normals, ship_indices, ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals);

	simple_ship = service_locator->return_render_manager()->create_object_buffers(ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals, ship_indices);
}

void Gameplay_Manager::load_base_projectile()
{
	// Vertices for the Ship Model
	std::vector<glm::vec3> ship_vertices;
	std::vector<glm::vec2> ship_uvs;
	std::vector<glm::vec3> ship_normals;
	std::vector<glm::vec3> ship_indexed_vertices;
	std::vector<glm::vec2> ship_indexed_uvs;
	std::vector<glm::vec3> ship_indexed_normals;
	std::vector<unsigned short> ship_indices;

	loadOBJ_no_uvs("Basic_Ship.obj", ship_vertices, ship_uvs, ship_normals);
	indexVBO(ship_vertices, ship_uvs, ship_normals, ship_indices, ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals);

	simple_projectile = service_locator->return_render_manager()->create_object_buffers(ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals, ship_indices);
}

void Gameplay_Manager::Remove_Hive_Ship_Array_From_Manifest(Hive_Ship_Array* ship_array)
{
	hive_ship_array_manifest.remove_hive_ship_array_from_manifest(ship_array);
}

void Gameplay_Manager::Create_New_Projectile(vec3 start_position, vec3 end_position)
{
	if (projectile_array.size() < 1000)
	{
		Projectile* new_projectile = service_locator->return_memory_manager()->new_projectile();

		new_projectile->Init(start_position, end_position);

		projectile_array.push_back(new_projectile);

		Update_Projectile_Color_Matrices();
	}
}

void Gameplay_Manager::Update_Projectile_Model_Matrices()
{
	int array_index = 0;

	for (int i = 0; i < projectile_array.size(); i++)
	{
		if (projectile_array[i]->is_init())
		{
			projectile_model_matrices[array_index] = projectile_array[i]->Return_Model_Matrix();
			array_index++;
		}
	}
}

void Gameplay_Manager::Update_Projectile_Color_Matrices()
{
	for (int i = 0; i < projectile_array.size(); i++)
	{
		projectile_color_matrices[i] = projectile_array[i]->Return_Color();
	}
}

void Gameplay_Manager::Set_Hive_Swarm_Engagement_Target(Hive_Ship_Array* swarm, Hive_Object* hive_engagement_target)
{
	swarm->set_hive_engagement_target(hive_engagement_target);
	swarm->set_array_state(Hive_Ship_Array::SWARM_STATE_COMBAT_ENGAGED);


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
			else hive_object->update();
		}
	}

	for (int i = 0; i < projectile_array.size(); i++)
	{
		projectile_array[i]->update();

		if (projectile_array[i]->Return_Lifespan() <= 0)
		{
			service_locator->return_memory_manager()->Deallocate_Projectile(projectile_array[i]);
			projectile_array.erase(projectile_array.begin() + i);
		}
	}

	Update_Projectile_Model_Matrices();
}



