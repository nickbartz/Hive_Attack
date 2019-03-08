#pragma once
#include<Object_Hive_Swarm.h>

class Ship_Object
{
public:
	enum ship_states
	{
		SHIP_STATE_IDLE,
		SHIP_STATE_COMBAT_ENGAGED,
		SHIP_STATE_MOVING_TO_POD,
		SHIP_STATE_CARRYING_POD
	};

	void Init(Service_Locator* service_locator, glm::vec3 start_location, vec3 _orbit_location);

	void add_engaged_ship(Ship_Object* ship_object);

	float calculate_distance_from_orbit_location();
	float calculate_total_rotation_from_goal(vec3 current_position, vec3 goal_position);
	float calculate_damage_from_engaging_ships();

	vec3 calculate_direction_vector_for_goal(vec3 current_transform, vec3 target_transform, vec3 current_direction_vector, float turning_circle);

	void decrement_ship_fire_cooldown();
	int decrement_num_plundered_pods(int decrement);
	void destroy();
	void double_check_engaged_ships();

	int get_major_array_id();

	void increment_rotation_matrix(vec3 incremental_rotation_vector);

	bool is_active();
	bool is_carrying_pod();

	void increment_transform_matrix(glm::vec3 current_direction_vector);
	int increment_num_plundered_pods(int increment);

	void move();

	void move_orbit();

	void move_combat();

	void process_ship_damage();

	void Reduce_Ship_Health(float health_decrement);

	void remove_hive_pod_plunder_target();
	void remove_ship_focus();
	void remove_engaged_ship(Ship_Object * ship_object);
	
	int return_ship_direction();
	int return_current_state();
	vec3 return_current_transform_vector();
	vec3 return_ship_directional_vector();
	Ship_Object* return_ship_combat_focus();
	vec3 return_current_rotation_vector();
	float return_orbit_distance();
	mat4 return_current_model_matrix();
	int return_num_plundered_pods();
	int return_matrix_array_id();
	vec3 return_home_orbit();
	vec3 return_closest_orbit_entry_point();
	float return_ship_health();
	float return_ship_damage();
	float return_ship_fire_cooldown();
	Hive_Pod_Object* return_current_hive_pod_focus();
	Hive_Pod_Object* return_current_hive_pod_owner();

	vec3 rotate_directional_vector(vec3 vector, float angle);

	void set_hive_pod_owner(Hive_Pod_Object* hive_pod_owner);
	void set_hive_pod_plunder_target(Hive_Pod_Object* hive_pod);
	void set_current_state(int current_state);
	void set_ship_combat_focus(Ship_Object* ship_focus);
	void set_is_carrying_pod(bool carrying);
	void set_major_array_id(int major_array_id);
	void set_matrix_array_id(int array_id);
	void set_active(bool _active);
	void set_new_orbit(glm::vec3 _orbit_location);
	void set_current_orbit_around_ship(Ship_Object* ship);
	void set_home_orbit(vec3 _home_orbit_location);
	void set_current_transform_vector(vec3 new_transform_vector);
	void set_ship_damage(float ship_damage);
	void set_ship_direction(int direction);
	void set_ship_fire_cooldown(float ship_fire_cooldown);

	void update_transform_matrix(glm::vec3 transform_vector);
	void update_rotation_matrix(glm::vec3 euler_rotation_vector);

private:
	Service_Locator * service_locator;

	Hive_Pod_Object* hive_pod_owner = NULL;
	Hive_Pod_Object* current_hive_pod_focus = NULL;
	Ship_Object* current_ship_focus = NULL;

	vector<Ship_Object*> ships_engaging;
	int major_array_id = 0;
	int matrix_array_id = 0;
	int ship_current_state = SHIP_STATE_IDLE;
	bool active = false;

	float ship_health = 100.0;
	float ship_damage = 0.5;

	float ship_fire_cooldown = 0.0;

	bool ship_is_carrying_pod = false;
	int num_plundered_pods = 0;

	float thrust = 1.0;
	float turning_circle = 0.02;

	vec3 current_transform_vector = { 0.0,0.0,0.0 };
	vec3 current_rotation_vector = { 0.0,0.0,0.0 };
	vec3 current_direction_vector = { 1.0,0.0,0.0 };

	vec3 orbit_location = { 0.0,0.0,0.0 };
	vec3 home_orbit_location = { 0.0,0.0,0.0 };
	float orbit_distance = 3;
	bool clock_wise = false;
	bool direction = rand() % 2;

	mat4 ScaleMatrix = scale(vec3{ 0.4f,0.4f,0.4f });
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);
	int ship_model_type = 0;

	model_buffer_specs loaded_specs;

};
