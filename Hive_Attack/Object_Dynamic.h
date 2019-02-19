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

const int MAX_SHIPS_PER_HIVE = 1000;

class Projectile
{
public:
	Projectile();
	void Init(vec3 start, vec3 finish);
	void Destroy();
	void update();
	void update_transform_matrix(glm::vec3 transform_vector);
	void Draw();
	void Decrement_Lifespan();
	int Return_Lifespan();
	mat4 Return_Model_Matrix();
	bool is_init();

private:
	int lifespan;

	vec3 start_position;
	vec3 end_position;
	vec3 current_transform_vector;
	vec3 direction_vector;

	mat4 ScaleMatrix = scale(vec3{ 0.1f,0.1f,0.1f });
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(1.0);

	float speed;

	bool initialized = false;

};

class Ship_Object
{
public:
	float speed = 0.05;

	vec3 current_transform_vector = { 0.0,0.0,0.0 };
	vec3 current_rotation_vector = { 0.0,0.0,0.0 };
	vec3 current_direction_vector = { speed,0.0,0.0 };

	vec3 orbit_location = { 0.0,0.0,0.0 };
	vec3 home_orbit_location = { 0.0,0.0,0.0 };
	float orbit_distance = 3;
	bool steady_orbit = false;
	bool clock_wise = false;
	bool direction = rand() % 2;

	float needed_rotation_angle = 0.0;

	mat4 ScaleMatrix = scale(vec3{ 0.4f,0.4f,0.4f });
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);
	int ship_model_type = 0;

	enum ship_states
	{
		SHIP_STATE_IDLE,
		SHIP_STATE_ENGAGED,
		SHIP_STATE_PLUNDERING
	};

	model_buffer_specs loaded_specs;

	bool is_active();

	void set_active(bool _active);

	int return_current_state();

	void set_current_state(int current_state);

	int get_major_array_id();

	void set_major_array_id(int major_array_id);

	void Init(Service_Locator* service_locator, glm::vec3 start_location, vec3 _orbit_location);

	void update_rotation_matrix(glm::vec3 euler_rotation_vector);

	void increment_rotation_matrix(vec3 incremental_rotation_vector);

	void update_transform_matrix(glm::vec3 transform_vector);

	void increment_transform_matrix(glm::vec3 current_direction_vector);

	vec3 return_current_transform_vector();

	vec3 return_current_rotation_vector();

	mat4 return_current_model_matrix();

	vec3 return_home_orbit();

	void rotate_directional_vector(float angle);

	void set_new_orbit(glm::vec3 _orbit_location);

	void set_home_orbit(vec3 _home_orbit_location);

	float calculate_distance_from_orbit_location();

	glm::vec3 return_closest_orbit_entry_point();

	void calculate_needed_rotation_increment_from_goal(glm::vec3 goal_position);

	void move();

	void check_nearby_targets();

	// HIVE POD FOCUS
	void set_hive_pod_plunder_target(Hive_Pod_Object* hive_pod);
	void remove_hive_pod_plunder_target();
	Hive_Pod_Object* return_current_hive_pod_target();
	bool is_carrying_pod();
	void set_is_carrying_pod(bool carrying);

	void set_ship_focus(Ship_Object* ship_focus);
	void remove_ship_focus();
	Ship_Object* return_ship_focus();

	void Reduce_Ship_Health(float health_decrement);

	float get_health();

	float return_ship_damage();

	void set_ship_damage(float ship_damage);

	void add_engaged_ship(Ship_Object* ship_object);

	void remove_engaged_ship(Ship_Object * ship_object);

	int return_ship_direction();
	void set_ship_direction(int direction);

	float return_ship_fire_cooldown();
	void set_ship_fire_cooldown(float ship_fire_cooldown);
	void decrement_ship_fire_cooldown();

	void double_check_engaged_ships();

	float calculate_damage_from_engaging_ships();

	int increment_num_plundered_pods(int increment);
	int decrement_num_plundered_pods(int decrement);
	int return_num_plundered_pods();

	void process_ship_damage();

private:
	Service_Locator * service_locator;

	Hive_Pod_Object* current_hive_pod_focus = NULL;
	Ship_Object* current_ship_focus = NULL;

	vector<Ship_Object*> ships_engaging;
	int major_array_id = 0;
	int ship_current_state = SHIP_STATE_IDLE;
	bool active = false;

	float ship_health = 100.0;
	float ship_damage = 0.5;

	float ship_fire_cooldown = 0.0;

	bool ship_is_carrying_pod = false;
	int num_plundered_pods = 0;

};

class Hive_Ship_Array
{
public:
	enum swarm_states
	{
		SWARM_STATE_IDLE,
		SWARM_STATE_ENGAGED,
		SWARM_STATE_PLUNDERING
	};

	Hive_Ship_Array();
	void init_hive_ship_array(Service_Locator* service_locator, vec3 ship_color, float ship_damage, model_buffer_specs* loaded_ship_specs);
	Ship_Object* add_ship_to_array(vec3 ship_location, vec3 ship_orbit_center);
	void remove_ship_from_array(Ship_Object* ship);
	Ship_Object* return_ship_in_array(int index);
	int return_ship_model_type();
	int return_uniq_id();
	int array_begin();
	int array_end();
	vec3 return_ship_array_color();
	Hive_Ship_Array* return_engaged_ship_array();

	int return_hive_array_state();
	model_buffer_specs* return_loaded_ship_specs();
	void set_engaged(bool engaged);
	void set_hive_engagement_target(Hive_Object* hive_target);
	void set_ship_array_to_idle();
	void set_array_state(int new_array_state);

	void set_ship_array_damage(float damage);
	float return_ship_array_damage();
	Hive_Pod_Object* find_hive_pod_engagement_focus();
	void send_ships_to_collect_enemy_hive_pods(Hive_Object* destroyed_hive);
	void update_ships();

	mat4* return_ships_model_matrices();
	void update_ships_color_matrices();
	vec3* return_ships_color_matrices();

	int return_num_ships_in_swarm();
	void increment_num_pods_to_add_to_hive(vec3 hive_color); // GOOD LORD THIS IS TEMPORARY
	void decrement_num_pods_to_add_to_hive(int decrement);
	int return_num_pods_to_add_to_hive();
	vector<vec3>* return_pods_to_add_to_hive();

private:
	Service_Locator * service_locator;

	int uniq_id;

	int hive_array_state = SWARM_STATE_IDLE;
	Hive_Ship_Array* ship_array_engagement_target = NULL;
	Hive_Object* hive_engagement_target = NULL;

	Ship_Object Ship_Object_Array[MAX_SHIPS_PER_HIVE];
	int ship_model_type;
	int num_ships_in_swarm = 0;
	int max_list_pointer = 1;

	vector<vec3> pods_to_add_to_hive;

	vec3 ship_array_color = { 0.75,0.75,0.0 };
	float ship_array_damage = 0.5;

	model_buffer_specs* loaded_ship_specs;
	mat4 ship_model_matrices[MAX_SHIPS_PER_HIVE];
	vec3 ship_color_matrices[MAX_SHIPS_PER_HIVE];
};

class Hive_Ship_Array_Manifest
{
public:
	Hive_Ship_Array_Manifest();
	void add_hive_ship_array_to_manifest(Hive_Ship_Array* octohedron_swarm);
	void remove_hive_ship_array_from_manifest(Hive_Ship_Array* octohedron_swarm);
	Hive_Ship_Array* Return_Ship_Array(int index);
	int Array_Begin();
	int Array_End();

private:
	vector<Hive_Ship_Array*> array_pointer_manifest;
};