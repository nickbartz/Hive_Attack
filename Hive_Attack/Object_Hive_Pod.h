#pragma once

#include<Object_Hive.h>

class Ship_Object;

class Hive_Pod_Object
{
public:
	Hive_Pod_Object()
	{
	}

	Grid_Coord octohedron_coordinates;
	Octohedron_Model* base_octohedron;

	vec3 scale_vector;
	vec3 local_translation_vector;
	vec3 world_translation_vector;
	vec3 rotation_vector;

	void destroy();

	int index_hash = rand() % 100000;

	void Init_Hive_Pod(Service_Locator* service_locator, glm::vec3 local_translation, vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset, vec3 pod_color);

	bool is_active();

	bool is_able_to_create_ship();

	void register_hive_ship(Ship_Object* hive_ship);

	Octohedron_Model* return_octohedron_base();

	float return_hive_pod_health();

	vec3 return_local_translation_vector();

	vec3 return_world_translation_vector();

	vec3 return_scaled_translation_vector();

	void remove_associated_ship_from_array(Ship_Object* ship);

	void set_active(bool active);

	void set_pod_is_being_plundered(bool is_being_plundered);

	void set_pod_color(vec3 pod_color);

	void set_current_matrix_pointer(int array_pointer);

	int return_current_matrix_pointer();

	vec3 return_pod_color();

	mat4 return_current_model_matrix();

	bool return_pod_is_being_plundered();

	void take_damage(float damage);

	void update_translation(vec3 local_translation, vec3 world_translation);

	void update();
	
private:
	Service_Locator * service_locator;

	vector<Ship_Object*> hive_ship_array;
	int max_num_pod_ships;
	int ship_build_counter = 0;
	int max_ship_build_counter;

	bool pod_is_active = false;
	int matrices_array_pointer = 0;
	float hive_pod_health = 100.0f;

	bool pod_is_being_plundered = false;

	mat4 ScaleMatrix = glm::mat4(1.0);;
	mat4 Transform_Matrix = translate(vec3{ 0.0f,0.0f,0.0f });
	mat4 RotationMatrix = glm::mat4(1.0);

	vec3 pod_color = { 1.0f,1.0f,1.0f };
};
