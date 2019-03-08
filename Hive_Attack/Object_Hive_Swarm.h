#pragma once

#include<iostream>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include<vector>
#include<map>
#include<iostream>
#include<Render_Manager.h>

using namespace std;
using namespace glm;

class Hive_Pod_Object;
class Hive_Object;
class Ship_Object;

const int MAX_SHIPS_PER_HIVE = 1000;

class Hive_Ship_Array
{
public:
	enum swarm_states
	{
		SWARM_STATE_IDLE,
		SWARM_STATE_SWARM_TARGET,
		SWARM_STATE_HIVE_TARGET
	};

	Hive_Ship_Array();	
	void init_hive_ship_array(Service_Locator* service_locator, vec3 ship_color, float ship_damage, model_buffer_specs* loaded_ship_specs);
	void destroy();
	int return_uniq_id();

	Ship_Object* add_new_ship_to_array(vec3 ship_location, vec3 ship_orbit_center);
	void add_existing_ship_to_array(Ship_Object* ship);
	void remove_ship_from_array(Ship_Object* ship);
	void delete_ship_from_array(Ship_Object* ship);
	int return_num_ships_in_swarm();
	int return_ship_model_type();
	Ship_Object * return_random_active_ship();
	Ship_Object* return_ship_by_array_num(int array_num);

	vec3 return_ship_array_color();
	bool is_active();
	model_buffer_specs* return_loaded_ship_specs();

	void change_swarm_target(Hive_Ship_Array * hive_ship_array, Hive_Object * hive_object);
	void remove_swarm_engagement_target();
	int return_current_state();

	Hive_Ship_Array* return_swarm_target();
	Hive_Pod_Object* find_hive_pod_engagement_focus();
	Hive_Object* return_hive_target();
	vec3 return_current_location();

	void set_ship_array_damage(float damage);
	float return_ship_array_damage();

	void update_ship_idle(Ship_Object * ship);
	void update_ship_engaged(Ship_Object * ship);
	void update_ship_plundering(Ship_Object * ship);

	void update();

	void reset_all_ships_in_swarm();

	void set_active(bool _active);

	mat4* update_all_ships_model_matrices();
	void update_single_ship_model_matrix(Ship_Object * ship);
	void update_ships_color_matrices();
	vec3* return_ships_color_matrices();

	void increment_num_pods_to_add_to_hive(vec3 hive_color); // GOOD LORD THIS IS TEMPORARY
	void decrement_num_pods_to_add_to_hive(int decrement);
	mat4 * return_model_matrix_array();
	int return_num_pods_to_add_to_hive();
	vector<vec3>* return_pods_to_add_to_hive();

private:
	Service_Locator * service_locator;

	vec3 current_swarm_location;

	bool active = false;

	int uniq_id;

	int swarm_array_state = SWARM_STATE_IDLE;
	Hive_Ship_Array* ship_array_target = NULL;
	Hive_Object* hive_target = NULL;

	vector<Ship_Object*> Ship_Object_Array;
	int ship_model_type;

	vector<vec3> pods_to_add_to_hive;

	vec3 ship_array_color = { 0.75,0.75,0.0 };
	float ship_array_damage = 0.5;

	model_buffer_specs* loaded_ship_specs;
	mat4 ship_model_matrices[MAX_SHIPS_PER_HIVE];
	vec3 ship_color_matrices[MAX_SHIPS_PER_HIVE];

	// INTERNAL SWARM AI FUNCTIONS
	bool check_swarm_target();
	bool check_hive_target();

};