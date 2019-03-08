#include<Object_Hive_Ship.h>
#include<unordered_set>
#include<glm/gtx/rotate_vector.hpp>

void Ship_Object::Init(Service_Locator* _service_locator, glm::vec3 start_location, vec3 _orbit_location)
{
	service_locator = _service_locator;

	update_transform_matrix(start_location);

	orbit_location = _orbit_location;
	home_orbit_location = _orbit_location;

	thrust = 0.05;
	turning_circle = (rand() % 20 + 10) / 1000.0f;

	ship_damage = 0.4;
	ship_health = 100.0;
}

void Ship_Object::add_engaged_ship(Ship_Object * ship_object)
{
	ships_engaging.push_back(ship_object);
}

float Ship_Object::calculate_distance_from_orbit_location()
{
	return distance(current_transform_vector, orbit_location);
}

float Ship_Object::calculate_damage_from_engaging_ships()
{
	float damage = 0.0;
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		vec3 ship_pos = return_current_transform_vector();
		vec3 engaging_ship_pos = ships_engaging[i]->return_current_transform_vector();

		float target_distance = distance(ship_pos, engaging_ship_pos);

		int int_distance = target_distance > 1 ? target_distance : 1;

		int hit_chance = rand() % int_distance;

		if (hit_chance <= 1)
		{
			damage += ships_engaging[i]->return_ship_damage();
		}
	}
	return damage;
}

vec3 Ship_Object::calculate_direction_vector_for_goal(vec3 current_transform, vec3 target_transform, vec3 current_direction_vector, float turning_circle)
{
	float needed_rotation = calculate_total_rotation_from_goal(current_transform, target_transform);

	int sign = needed_rotation / abs(needed_rotation);

	float final_rotation;

	if (turning_circle < abs(needed_rotation))
	{
		final_rotation = sign * turning_circle;
	}
	else final_rotation = needed_rotation;

	vec3 needed_direction_vector = normalize(rotate_directional_vector(current_direction_vector, final_rotation));

	return needed_direction_vector;
}

float Ship_Object::calculate_total_rotation_from_goal(vec3 current_position, vec3 goal_position)
{
	float x_1 = current_direction_vector.x;
	float z_1 = current_direction_vector.z;

	vec2 goal_direction_vector = { goal_position.x - current_position.x,goal_position.z - current_position.z };
	goal_direction_vector = normalize(goal_direction_vector);

	float dot = x_1 * goal_direction_vector.x + z_1 * goal_direction_vector.y;
	float det = x_1 * goal_direction_vector.y - z_1 * goal_direction_vector.x;

	float angle = atan2(det, dot);

	if (isnan(angle)) return 0.0f;
	else return angle;
}

void Ship_Object::double_check_engaged_ships()
{
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		if (ships_engaging[i]->is_active() == false)
		{
			ships_engaging.erase(ships_engaging.begin() + i);
		}
	}
}

int Ship_Object::decrement_num_plundered_pods(int decrement)
{
	num_plundered_pods -= decrement;
	return num_plundered_pods;
}

void Ship_Object::destroy()
{
	active = false;
}

float Ship_Object::return_ship_health()
{
	return ship_health;
}

int Ship_Object::get_major_array_id()
{
	return major_array_id;
}

bool Ship_Object::is_active()
{
	return active;
}

void Ship_Object::increment_rotation_matrix(vec3 incremental_rotation_vector)
{
	current_rotation_vector += incremental_rotation_vector;
	update_rotation_matrix(current_rotation_vector);
}

void Ship_Object::increment_transform_matrix(glm::vec3 current_direction_vector)
{
	glm::vec3 new_transform_vector = current_transform_vector + current_direction_vector;
	current_transform_vector = new_transform_vector;
	update_transform_matrix(current_transform_vector);
}

int Ship_Object::increment_num_plundered_pods(int increment)
{
	num_plundered_pods += increment;
	return num_plundered_pods;
}

void Ship_Object::move()
{ 
	if (return_ship_combat_focus() == NULL) move_orbit();
	else move_combat();

	// Adjust Height if Necessary
	float height_difference = abs(orbit_location.y - current_transform_vector.y);
	int height_direction = height_difference / (orbit_location.y - current_transform_vector.y);

	if (height_difference >= thrust) current_direction_vector.y = thrust * height_direction;
	else current_direction_vector.y = orbit_location.y - current_transform_vector.y;

	increment_transform_matrix(current_direction_vector*thrust);
}

void Ship_Object::move_orbit()
{
	float distance = calculate_distance_from_orbit_location();

	if (distance > orbit_distance && distance < orbit_distance*1.5)
	{
		float body_pos_x = current_transform_vector.x;
		float body_pos_z = current_transform_vector.z;

		float angle = atan2(body_pos_z - orbit_location.z, body_pos_x - orbit_location.x);

		float body_vel_x = (sin(angle));
		float body_vel_y = (cos(angle));

		if (direction) current_direction_vector = { body_vel_x, 0.0, -body_vel_y };
		else current_direction_vector = { -body_vel_x, 0.0, body_vel_y };

		current_direction_vector = normalize(current_direction_vector);

		//vec3 origin_direction = normalize(orbit_location - current_transform_vector);

		//current_direction_vector += origin_direction*0.01f;
	}
	else if (distance > orbit_distance*1.5)
	{
		current_direction_vector = calculate_direction_vector_for_goal(current_transform_vector, return_closest_orbit_entry_point(), current_direction_vector, turning_circle);
	}

	// FIX THIS SO THE SHIP IS ALWAYS POINTIN IN THE DIRECTION OF ITS TARGET WHILE FIGHTING AND PLUNDERING, NOT ITS ORBIT
	float rotation_angle = atan2f(current_direction_vector.z, current_direction_vector.x);

	update_rotation_matrix({ rotation_angle, 0.0,-1.55 });
}

void Ship_Object::move_combat()
{
	vec3 target_location = current_ship_focus->return_current_transform_vector();
	vec3 target_direction = current_ship_focus->return_ship_directional_vector();

	vec3 ship_target = target_location - target_direction;

	current_direction_vector = calculate_direction_vector_for_goal(current_transform_vector, ship_target, current_direction_vector, turning_circle);

	// Set ships rotational_direction
	vec3 direction_to_combat_focus = normalize(target_location - current_transform_vector);

	float rotation_angle = atan2f(direction_to_combat_focus.z, direction_to_combat_focus.x);

	update_rotation_matrix({ rotation_angle, 0.0,-1.55 });
}


void Ship_Object::process_ship_damage()
{
	double_check_engaged_ships();

	calculate_damage_from_engaging_ships();

	Reduce_Ship_Health(calculate_damage_from_engaging_ships());
}

vec3 Ship_Object::return_current_transform_vector()
{
	return current_transform_vector;
}

vec3 Ship_Object::return_ship_directional_vector()
{
	return current_direction_vector;
}

vec3 Ship_Object::return_current_rotation_vector()
{
	return current_rotation_vector;
}

float Ship_Object::return_orbit_distance()
{
	return orbit_distance;
}

mat4 Ship_Object::return_current_model_matrix()
{
	return Transform_Matrix * RotationMatrix*ScaleMatrix;
}

int Ship_Object::return_matrix_array_id()
{
	return matrix_array_id;
}

int Ship_Object::return_current_state()
{
	return ship_current_state;
}

vec3 Ship_Object::return_home_orbit()
{
	return home_orbit_location;
}

vec3 Ship_Object::return_closest_orbit_entry_point()
{
	// DEPRECATED
	float radius = orbit_distance;

	float px = current_transform_vector.x;
	float pz = current_transform_vector.z;

	float cx = orbit_location.x;
	float cz = orbit_location.z;

	//cout << px << ", " << pz << " : " << cx << ", " << cz << endl;

	float dx = cx - px;
	float dz = cz - pz;

	if (abs(dx) > 0 || abs(dz) > 0)
	{
		float dd = sqrt(dx * dx + dz * dz);

		float a = asin(radius / dd);

		float b = atan2(dz, dx);

		float alpha = b - a;
		float xa = radius * sin(alpha);
		float za = radius * -cos(alpha);

		float theta = b + a;
		float xb = radius * -sin(theta);
		float zb = radius * cos(theta);

		float x_1 = cx + xa;
		float z_1 = cz + za;

		float x_2 = cx + xb;
		float z_2 = cz + zb;

		float current_angle = atan2(current_direction_vector.z, current_direction_vector.x);
		float tangent_angle_1 = atan2((z_1 - pz), (x_1 - px));
		float tangent_angle_2 = atan2((z_2 - pz), (x_2 - px));

		vec3 orbit_entry_point;

		float tangent_current_1 = 3.14158 - abs(abs(tangent_angle_1 - current_angle) - 3.14158);
		float tangent_current_2 = 3.14158 - abs(abs(tangent_angle_2 - current_angle) - 3.14158);

		if (tangent_current_1 <= tangent_current_2)
		{
			orbit_entry_point = { x_1, 0.0, z_1 };
		}
		else
		{
			orbit_entry_point = { x_2, 0.0, z_2 };
		}

		return orbit_entry_point;
	}

	else
	{
		return current_transform_vector;
	}

}

void Ship_Object::remove_hive_pod_plunder_target()
{
	current_hive_pod_focus = NULL;
}

void Ship_Object::remove_engaged_ship(Ship_Object* ship_object)
{
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		if (ships_engaging[i] == ship_object) ships_engaging.erase(ships_engaging.begin() + i);
	}
}

int Ship_Object::return_ship_direction()
{
	return direction;
}

void Ship_Object::set_matrix_array_id(int array_id)
{
	matrix_array_id = array_id;
}

void Ship_Object::set_ship_direction(int _direction)
{
	direction = _direction;
}

float Ship_Object::return_ship_fire_cooldown()
{
	return ship_fire_cooldown;
}

void Ship_Object::set_ship_fire_cooldown(float _ship_fire_cooldown)
{
	ship_fire_cooldown = _ship_fire_cooldown;
}

void Ship_Object::decrement_ship_fire_cooldown()
{
	ship_fire_cooldown -= 1.0f;

	if (ship_fire_cooldown <= 0.0) ship_fire_cooldown = 0.0;
}

float Ship_Object::return_ship_damage()
{
	return ship_damage;
}

Hive_Pod_Object * Ship_Object::return_current_hive_pod_focus()
{
	return current_hive_pod_focus;
}

Hive_Pod_Object * Ship_Object::return_current_hive_pod_owner()
{
	return hive_pod_owner;
}

bool Ship_Object::is_carrying_pod()
{
	return ship_is_carrying_pod;
}

void Ship_Object::set_is_carrying_pod(bool carrying)
{
	ship_is_carrying_pod = carrying;
}

void Ship_Object::set_ship_combat_focus(Ship_Object * ship_focus)
{
	current_ship_focus = ship_focus;
}

void Ship_Object::remove_ship_focus()
{
	current_ship_focus = NULL;
}

Ship_Object * Ship_Object::return_ship_combat_focus()
{
	return current_ship_focus;
}

void Ship_Object::Reduce_Ship_Health(float health_decrement)
{
	if (ship_health > 0.0)
	{
		ship_health -= health_decrement;
	}
}

vec3 Ship_Object::rotate_directional_vector(vec3 vector, float angle)
{
	float x_coord = ((vector.x) * cos(angle)) - ((vector.z)*sin(angle));
	float z_coord = ((vector.z) * cos(angle)) + ((vector.x)*sin(angle));

	vector.x = x_coord;
	vector.z = z_coord;

	return vector;
}

int Ship_Object::return_num_plundered_pods()
{
	return num_plundered_pods;
}

void Ship_Object::set_new_orbit(glm::vec3 _orbit_location)
{
	orbit_location = _orbit_location;
}

void Ship_Object::set_current_orbit_around_ship(Ship_Object * ship)
{
	if (ship->return_ship_direction() == 1) set_ship_direction(1);
	else set_ship_direction(0);



	set_new_orbit(ship->current_transform_vector);
}

void Ship_Object::set_home_orbit(vec3 _home_orbit_location)
{
	home_orbit_location = _home_orbit_location;
}

void Ship_Object::set_current_transform_vector(vec3 new_transform_vector)
{
	current_transform_vector = new_transform_vector;
}

void Ship_Object::set_current_state(int current_state)
{
	ship_current_state = current_state;
}

void Ship_Object::set_major_array_id(int _major_array_id)
{
	major_array_id = _major_array_id;
}

void Ship_Object::set_active(bool _active)
{
	active = _active;
}

void Ship_Object::set_ship_damage(float _ship_damage)
{
	ship_damage = _ship_damage;
}

void Ship_Object::set_hive_pod_owner(Hive_Pod_Object * _hive_pod_owner)
{
	hive_pod_owner = _hive_pod_owner;
}

void Ship_Object::set_hive_pod_plunder_target(Hive_Pod_Object * hive_pod)
{
	current_hive_pod_focus = hive_pod;
}

void Ship_Object::update_rotation_matrix(glm::vec3 euler_rotation_vector)
{
	current_rotation_vector = euler_rotation_vector;
	glm::quat MyQuaternion;
	glm::vec3 EulerAngles(current_rotation_vector);
	MyQuaternion = glm::quat(EulerAngles);
	RotationMatrix = glm::toMat4(MyQuaternion);
}

void Ship_Object::update_transform_matrix(glm::vec3 transform_vector)
{
	Transform_Matrix = glm::translate(transform_vector);
	current_transform_vector = transform_vector;
}

