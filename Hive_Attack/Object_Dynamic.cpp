#include <Object_Dynamic.h>
#include<Gameplay_Manager.h>
#include<Object_Static.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Render_Manager.h>
#include<Service_Locator.h>

// PROJECTILE

Projectile::Projectile()
{

}

void Projectile::Draw()
{

}

void Projectile::Decrement_Lifespan()
{
	lifespan--;

	if (lifespan <= 0) lifespan = 0;
}

int Projectile::Return_Lifespan()
{
	return lifespan;
}

mat4 Projectile::Return_Model_Matrix()
{
	return Transform_Matrix * RotationMatrix*ScaleMatrix;
}

bool Projectile::is_init()
{
	return initialized;
}

void Projectile::Init(vec3 start, vec3 finish)
{
	start_position = start;
	end_position = finish;
	current_transform_vector = start_position;
	direction_vector = end_position - start_position;
	speed = 0.05;

	float distance = sqrt(direction_vector.x*direction_vector.x + direction_vector.z*direction_vector.z);

	lifespan = distance/speed;
	initialized = true;
}

void Projectile::Destroy()
{
	initialized = false;
}

void Projectile::update()
{
	current_transform_vector += normalize(direction_vector) * speed;
	update_transform_matrix(current_transform_vector);

	Decrement_Lifespan();
}

void Projectile::update_transform_matrix(glm::vec3 transform_vector)
{
	Transform_Matrix = glm::translate(transform_vector);
	current_transform_vector = transform_vector;
}


// HIVE SHIP //

void Ship_Object::Init(Service_Locator* _service_locator, glm::vec3 start_location, vec3 _orbit_location)
{
	service_locator = _service_locator;
	
	update_transform_matrix(start_location);

	orbit_location = _orbit_location;
	home_orbit_location = _orbit_location;

	int speed_rand = rand() % 5 + 5;

	speed = speed_rand / 250.0;
	ship_damage = 0.1;
}

void Ship_Object::add_engaged_ship(Ship_Object * ship_object)
{
	ships_engaging.push_back(ship_object);
}

float Ship_Object::calculate_distance_from_orbit_location()
{
	float px = current_transform_vector.x;
	float pz = current_transform_vector.z;
	float cx = orbit_location.x;
	float cz = orbit_location.z;

	float dx = abs(cx - px);
	float dz = abs(cz - pz);
	return sqrt(dx * dx + dz * dz);
}

float Ship_Object::calculate_distance_between_two_points(float x1, float z1, float x2, float z2)
{
	float dx = abs(x2 - x1);
	float dz = abs(z2 - z1);

	float dd = sqrt(dx * dx + dz * dz);

	return dd;
}

float Ship_Object::calculate_damage_from_engaging_ships()
{
	float damage = 0.0;
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		vec3 ship_pos = return_current_transform_vector();
		vec3 engaging_ship_pos = ships_engaging[i]->return_current_transform_vector();

		float distance = calculate_distance_between_two_points(ship_pos.x, ship_pos.z, engaging_ship_pos.x, engaging_ship_pos.z);

		int int_distance = distance > 1 ? distance : 1;

		int hit_chance = rand() % int_distance;

		if (hit_chance <= 1)
		{
			damage += ships_engaging[i]->return_ship_damage();
		}
	}
	return damage;
}

void Ship_Object::check_nearby_targets()
{

}

void Ship_Object::calculate_needed_rotation_increment_from_goal(glm::vec3 goal_position)
{
	float x_1 = current_direction_vector.x;
	float z_1 = current_direction_vector.z;

	vec2 goal_direction_vector = { goal_position.x - current_transform_vector.x,goal_position.z - current_transform_vector.z };
	goal_direction_vector = normalize(goal_direction_vector);

	float dot = x_1 * goal_direction_vector.x + z_1 * goal_direction_vector.y;
	float det = x_1 * goal_direction_vector.y - z_1 * goal_direction_vector.x;

	float angle = atan2(det, dot);

	if (isnan(angle)) needed_rotation_angle = 0.0;
	else needed_rotation_angle = angle;
}

void Ship_Object::double_check_engaged_ships()
{
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		if (ships_engaging[i]->is_active() == false) ships_engaging.erase(ships_engaging.begin() + i);
	}
}

int Ship_Object::decrement_num_plundered_pods(int decrement)
{
	num_plundered_pods -= decrement;
	return num_plundered_pods;
}

float Ship_Object::get_health()
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
	float distance = calculate_distance_from_orbit_location();

	if (distance < orbit_distance)
	{
		steady_orbit = false;
	}
	else if (distance > orbit_distance && distance < orbit_distance*1.5)
	{
		float body_pos_x = current_transform_vector.x;
		float body_pos_z = current_transform_vector.z;

		float angle = atan2(body_pos_z - orbit_location.z, body_pos_x - orbit_location.x);
		
		float body_vel_x = (sin(angle) * speed);
		float body_vel_y = (cos(angle) * speed);

		if (direction) current_direction_vector = { body_vel_x, 0.0, -body_vel_y };
		else current_direction_vector = { -body_vel_x, 0.0, body_vel_y };

		vec3 origin_direction = orbit_location - current_transform_vector;
		origin_direction *= distance / orbit_distance * 0.0001;

		// Move the ship back towards its stable orbit if it's moving further away 
		current_direction_vector += origin_direction;
	}
	else if (distance > orbit_distance*1.5)
	{
		vec3 closest_entry_point = return_closest_orbit_entry_point();
		calculate_needed_rotation_increment_from_goal(closest_entry_point);
		rotate_directional_vector((needed_rotation_angle));
		steady_orbit = false;
	}

	// Adjust Height if Necessary
	float height_difference = abs(orbit_location.y - current_transform_vector.y);
	int height_direction = height_difference / (orbit_location.y - current_transform_vector.y);

	if (height_difference >= speed) current_direction_vector.y = speed * height_direction;
	else current_direction_vector.y = orbit_location.y - current_transform_vector.y;

	increment_transform_matrix(current_direction_vector);

	float rotation_angle = atan2f(current_direction_vector.z, current_direction_vector.x);

	if (return_current_state() == SHIP_STATE_ENGAGED || return_current_state() == SHIP_STATE_PLUNDERING)
	{
		if (direction) rotation_angle -= 3.14158 / 2;
		else rotation_angle += 3.14158 / 2;
	}

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

vec3 Ship_Object::return_current_rotation_vector()
{
	return current_rotation_vector;
}

mat4 Ship_Object::return_current_model_matrix()
{
	return Transform_Matrix * RotationMatrix*ScaleMatrix;
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

void Ship_Object::remove_engagement_target()
{
	current_engagement_focus->remove_engaged_ship(this);
	orbit_location = home_orbit_location;
	current_engagement_focus = NULL;
}

Ship_Object* Ship_Object::return_current_engagement_target()
{
	return current_engagement_focus;
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

Hive_Pod_Object * Ship_Object::return_current_hive_pod_target()
{
	return current_hive_pod_focus;
}

void Ship_Object::Reduce_Ship_Health(float health_decrement)
{
	if (ship_health > 0.0)
	{
		ship_health -= health_decrement;
	}
}

void Ship_Object::rotate_directional_vector(float angle)
{
	float x_coord = ((current_direction_vector.x) * cos(angle)) - ((current_direction_vector.z)*sin(angle));
	float z_coord = ((current_direction_vector.z) * cos(angle)) + ((current_direction_vector.x)*sin(angle));

	current_direction_vector.x = x_coord;
	current_direction_vector.z = z_coord;
}

int Ship_Object::return_num_plundered_pods()
{
	return num_plundered_pods;
}

void Ship_Object::set_new_orbit(glm::vec3 _orbit_location)
{
	orbit_location = _orbit_location;
	steady_orbit = false;
}

void Ship_Object::set_home_orbit(vec3 _home_orbit_location)
{
	home_orbit_location = _home_orbit_location;
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

void Ship_Object::set_engagement_target(Ship_Object* ship_object)
{
	if (ship_object != NULL)
	{
		current_engagement_focus = ship_object;
		ship_object->add_engaged_ship(this);
		orbit_location = current_engagement_focus->return_current_transform_vector();
		if (current_engagement_focus->direction == 0) direction = 0;
		else direction = 1;
	}
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


// HIVE SHIP ARRAY MANIFEST //

Hive_Ship_Array_Manifest::Hive_Ship_Array_Manifest()
{

}

void Hive_Ship_Array_Manifest::add_hive_ship_array_to_manifest(Hive_Ship_Array* octohedron_ship_array_pointer)
{
	array_pointer_manifest.push_back(octohedron_ship_array_pointer);
}

void Hive_Ship_Array_Manifest::remove_hive_ship_array_from_manifest(Hive_Ship_Array* octohedron_ship_array_pointer)
{
	for (int i = 0; i < array_pointer_manifest.size(); i++)
	{
		if (array_pointer_manifest[i] = octohedron_ship_array_pointer) array_pointer_manifest.erase(array_pointer_manifest.begin() + i);
	}
}

Hive_Ship_Array* Hive_Ship_Array_Manifest::Return_Ship_Array(int index)
{
	if (index < array_pointer_manifest.size() && index >= 0)
	{
		return array_pointer_manifest[index];
	}
}

int Hive_Ship_Array_Manifest::Array_Begin()
{
	return 0;
}

int Hive_Ship_Array_Manifest::Array_End()
{
	return array_pointer_manifest.size();
}


// HIVE SHIP ARRAY //

Hive_Ship_Array::Hive_Ship_Array()
{
	uniq_id = rand() % 1000;
}

void Hive_Ship_Array::init_hive_ship_array(Service_Locator* _service_locator, vec3 ship_color, float ship_damage, model_buffer_specs* _loaded_ship_specs)
{
	service_locator = _service_locator;

	ship_array_color = ship_color;
	ship_array_damage = ship_damage;

	loaded_ship_specs = _loaded_ship_specs;
}

int Hive_Ship_Array::array_begin()
{
	return 0;
}

int Hive_Ship_Array::array_end()
{
	return max_list_pointer - 1;
}

Ship_Object* Hive_Ship_Array::add_ship_to_array(vec3 current_location, vec3 orbit_location)
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Ship_Object_Array[i].is_active() == false)
		{
			Ship_Object new_ship;
			new_ship.Init(service_locator, current_location, orbit_location);
			new_ship.home_orbit_location = orbit_location;
			new_ship.set_ship_damage(return_ship_array_damage());
			new_ship.set_major_array_id(i);
			Ship_Object_Array[i] = new_ship;
			Ship_Object_Array[i].set_active(true);
			if (i + 1 == max_list_pointer) max_list_pointer++;
			num_ships_in_swarm++;
			return &Ship_Object_Array[i];
		}
	}
}

Ship_Object * Hive_Ship_Array::find_ship_engagement_target()
{
	if (ship_array_engagement_target != NULL)
	{
		Ship_Object* target = ship_array_engagement_target->return_ship_in_array(rand() % ship_array_engagement_target->array_end());
		if (target != NULL)
		{
			return target;
		}
	}
}

void Hive_Ship_Array::send_ships_to_collect_enemy_hive_pods(Hive_Object * destroyed_hive)
{
	if (destroyed_hive != NULL)
	{
		hive_array_state = SWARM_STATE_PLUNDERING;
		hive_engagement_target = destroyed_hive;
	}
}

int Hive_Ship_Array::return_hive_array_state()
{
	return hive_array_state;
}

model_buffer_specs* Hive_Ship_Array::return_loaded_ship_specs()
{
	return loaded_ship_specs;
}

void Hive_Ship_Array::remove_ship_from_array(Ship_Object* ship)
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Ship_Object_Array[i].get_major_array_id() == ship->get_major_array_id() && Ship_Object_Array[i].is_active() == true)
		{
			Ship_Object_Array[i].set_active(false);
			if (i + 1 == max_list_pointer) max_list_pointer--;
			num_ships_in_swarm--;
			break;
		}
	}
}

Ship_Object* Hive_Ship_Array::return_ship_in_array(int index)
{
	if (Ship_Object_Array[index].is_active() == true) return &Ship_Object_Array[index];
	else return NULL;
}

int Hive_Ship_Array::return_ship_model_type()
{
	return ship_model_type;
}

int Hive_Ship_Array::return_uniq_id()
{
	return uniq_id;
}

vec3 Hive_Ship_Array::return_ship_array_color()
{
	return ship_array_color;
}

float Hive_Ship_Array::return_ship_array_damage()
{
	return ship_array_damage;
}

mat4* Hive_Ship_Array::return_ships_model_matrices()
{
	int ship_index = 0;

	for (int i = 0; i < max_list_pointer; i++)
	{
		Ship_Object* new_ship = return_ship_in_array(i);
		if (new_ship != NULL && new_ship->is_active())
		{
			ship_model_matrices[ship_index] = new_ship->return_current_model_matrix();
			ship_index++;
		}
	}

	return ship_model_matrices;
}

int Hive_Ship_Array::return_num_ships_in_swarm()
{
	return num_ships_in_swarm;
}

void Hive_Ship_Array::increment_num_pods_to_add_to_hive(int increment)
{
	num_pods_to_add_to_hive += increment;
}

void Hive_Ship_Array::decrement_num_pods_to_add_to_hive(int decrement)
{
	num_pods_to_add_to_hive -= decrement;
}

int Hive_Ship_Array::return_num_pods_to_add_to_hive()
{
	return num_pods_to_add_to_hive;
}

Hive_Ship_Array* Hive_Ship_Array::return_engaged_ship_array()
{
	return ship_array_engagement_target;
}

void Hive_Ship_Array::set_ship_array_to_idle()
{
	hive_array_state = SWARM_STATE_IDLE;
}

void Hive_Ship_Array::set_array_state(int new_array_state)
{
	hive_array_state = SWARM_STATE_ENGAGED;
}

void Hive_Ship_Array::set_ship_array_damage(float damage)
{
	ship_array_damage = damage;
}

void Hive_Ship_Array::set_engaged(bool _engaged)
{
	if (_engaged == true)
	{
		hive_array_state = SWARM_STATE_ENGAGED;
	}
	else
	{
		hive_array_state = SWARM_STATE_IDLE;
	}

}

void Hive_Ship_Array::set_hive_engagement_target(Hive_Object * hive_target)
{
	hive_engagement_target = hive_target;
	ship_array_engagement_target = hive_target->return_hive_ship_array();
	set_engaged(true);
}

void Hive_Ship_Array::update_ships()
{
	for (int i = 0; i < array_end(); i++)
	{
		Ship_Object* ship = return_ship_in_array(i);

		if (ship != NULL)
		{
			if (return_hive_array_state() == SWARM_STATE_IDLE)
			{
				ship->set_new_orbit(ship->return_home_orbit());
				ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);

				if (ship->return_num_plundered_pods() > 0)
				{
					increment_num_pods_to_add_to_hive(ship->return_num_plundered_pods());

					ship->decrement_num_plundered_pods(ship->return_num_plundered_pods());
				}

			}
			else if (return_hive_array_state() == SWARM_STATE_ENGAGED)
			{
				// IF THE SHIP IS CURRENTLY NOT ENGAGED, FIND AND ENGAGEMENT TARGET
				if (ship->return_current_state() == Ship_Object::SHIP_STATE_IDLE)
				{
					Ship_Object* engaged_ship = find_ship_engagement_target();

					if (engaged_ship != NULL)
					{
						// SET THE SHIP'S ENGAGEMENT TARGET TO THE FOUND SHIP
						ship->set_current_state(Ship_Object::SHIP_STATE_ENGAGED);
						ship->set_engagement_target(engaged_ship);

						//IF THE NEW ENGAGEMENT TARGET ISN'T ALREADY ENGAGED, RECIPROCALLY ENGAGE IT 
						if (engaged_ship->return_current_state() == Ship_Object::SHIP_STATE_IDLE)
						{
							engaged_ship->set_current_state(Ship_Object::SHIP_STATE_ENGAGED);
							engaged_ship->set_engagement_target(ship);
						}
					}
				}
				// IF THE SHIP IS ENGAGED, BUT ITS ENGAGEMENT IS NO LONGER AROUND
				else if (ship->return_current_state() == Ship_Object::SHIP_STATE_ENGAGED && (ship->return_current_engagement_target() == NULL || ship->return_current_engagement_target()->get_health() <= 0.0))
				{
					ship->remove_engagement_target();
					ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
					ship->set_new_orbit(ship->return_home_orbit());
				}
			}
			// MANAGE PLUNDER STATE
			else if (return_hive_array_state() == SWARM_STATE_PLUNDERING)
			{
				if (hive_engagement_target->return_num_current_pods() <= 0) set_ship_array_to_idle();
				else if (hive_engagement_target != NULL && hive_engagement_target->return_num_current_pods() > 0)
				{
					if (ship->return_current_hive_pod_target() == NULL)
					{
						Hive_Pod_Object* pod_to_plunder = hive_engagement_target->return_random_active_pod();

						if (pod_to_plunder != NULL)
						{
							ship->set_new_orbit(pod_to_plunder->return_world_translation_vector());

							ship->set_hive_pod_plunder_target(pod_to_plunder);

							ship->set_current_state(Ship_Object::SHIP_STATE_PLUNDERING);
						}
					}
					else if (ship->return_current_hive_pod_target() != NULL)
					{
						if (ship->return_current_hive_pod_target()->return_hive_pod_health() > 0.0f)
						{
							// NEED TO UPDATE THIS FUNCTION TO MIRROR THE WAY IN WHICH SHIPS TAKE DAMAGE
							ship->return_current_hive_pod_target()->take_damage(ship->return_ship_damage());

							if (ship->return_current_hive_pod_target()->return_hive_pod_health() <= 0.0f) ship->increment_num_plundered_pods(1);
						}
						else if (ship->return_current_hive_pod_target()->return_hive_pod_health() <= 0.0f)
						{
							ship->remove_hive_pod_plunder_target();
						}
					}
				}
			}

			ship->process_ship_damage();

			if (return_hive_array_state() == SWARM_STATE_ENGAGED && ship->return_current_engagement_target() != NULL)
			{
				if (ship->return_ship_fire_cooldown() == 0.0)
				{
					service_locator->return_gameplay_manager()->Create_New_Projectile(ship->current_transform_vector, ship->return_current_engagement_target()->current_transform_vector);
					ship->set_ship_fire_cooldown(100.0);
				}
				else ship->decrement_ship_fire_cooldown();

				ship->set_new_orbit(ship->return_current_engagement_target()->current_transform_vector);
			}

			ship->move();

			if (ship->get_health() <= 0.0) remove_ship_from_array(ship);
		}
	}
}

