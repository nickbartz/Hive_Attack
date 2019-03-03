#include <Object_Hive_Swarm.h>
#include<Gameplay_Manager.h>
#include<Object_Hive.h>
#include<Object_Hive_Ship.h>
#include<Object_Hive_Pod.h>

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
#include<Memory_Manager.h>


// HIVE SHIP ARRAY //

Hive_Ship_Array::Hive_Ship_Array()
{
	uniq_id = rand() % 1000;
}

void Hive_Ship_Array::init_hive_ship_array(Service_Locator* _service_locator, vec3 ship_color, float ship_damage, model_buffer_specs* _loaded_ship_specs)
{
	active = true;

	service_locator = _service_locator;

	ship_array_color = ship_color;
	ship_array_damage = ship_damage;

	loaded_ship_specs = _loaded_ship_specs;
}

void Hive_Ship_Array::destroy()
{
	set_active(false);
}

Ship_Object* Hive_Ship_Array::add_ship_to_array(vec3 current_location, vec3 orbit_location)
{
	Ship_Object* new_ship = service_locator->return_memory_manager()->new_ship();

	new_ship->Init(service_locator, current_location, orbit_location);
	new_ship->set_ship_damage(return_ship_array_damage());
	new_ship->set_active(true);
	
	Ship_Object_Array.push_back(new_ship);

	update_all_ships_model_matrices();
	update_ships_color_matrices();

	return new_ship;
}

Hive_Pod_Object * Hive_Ship_Array::find_hive_pod_engagement_focus()
{
	if (hive_engagement_target != NULL)
	{
		Hive_Pod_Object* target = hive_engagement_target->return_random_active_pod();
		if (target != NULL)
		{
			return target;
		}
	}

	return NULL;
}

void Hive_Ship_Array::increment_num_pods_to_add_to_hive(vec3 pod_color)
{
	pods_to_add_to_hive.push_back(pod_color);
}

void Hive_Ship_Array::decrement_num_pods_to_add_to_hive(int decrement)
{
	for (int i = 0; i < decrement; i++)
	{
		pods_to_add_to_hive.pop_back();
	}
}

mat4* Hive_Ship_Array::return_model_matrix_array()
{
	return ship_model_matrices;
}

int Hive_Ship_Array::return_hive_array_state()
{
	return swarm_array_state;
}

bool Hive_Ship_Array::is_active()
{
	return active;
}

model_buffer_specs* Hive_Ship_Array::return_loaded_ship_specs()
{
	return loaded_ship_specs;
}

void Hive_Ship_Array::remove_ship_from_array(Ship_Object* ship)
{
	ship->return_current_hive_pod_owner()->remove_associated_ship_from_array(ship);

	for (int i = 0; i < Ship_Object_Array.size(); i++)
	{
		if (Ship_Object_Array[i] == ship)
		{
			Ship_Object_Array.erase(Ship_Object_Array.begin() + i);
			service_locator->return_memory_manager()->Deallocate_Ship(ship);
			break;
		}
	}

	update_all_ships_model_matrices();
	update_ships_color_matrices();
}

int Hive_Ship_Array::return_ship_model_type()
{
	return ship_model_type;
}

int Hive_Ship_Array::return_uniq_id()
{
	return uniq_id;
}

vec3 * Hive_Ship_Array::return_ships_color_matrices()
{
	return ship_color_matrices;
}

int Hive_Ship_Array::return_num_ships_in_swarm()
{
	return Ship_Object_Array.size();
}

int Hive_Ship_Array::return_num_pods_to_add_to_hive()
{
	return pods_to_add_to_hive.size();
}

vector<vec3>* Hive_Ship_Array::return_pods_to_add_to_hive()
{
	return &pods_to_add_to_hive;
}

Hive_Ship_Array* Hive_Ship_Array::return_engaged_ship_array()
{
	return ship_array_engagement_target;
}

int Hive_Ship_Array::return_current_state()
{
	return swarm_array_state;
}

Ship_Object* Hive_Ship_Array::return_random_active_ship()
{
	if (Ship_Object_Array.size() > 0)
	{
		int array_num = rand() % Ship_Object_Array.size();

		Ship_Object* ship = Ship_Object_Array[array_num];

		if (ship != NULL && ship->is_active() == true) return ship;
	}
	else return NULL;
}

vec3 Hive_Ship_Array::return_ship_array_color()
{
	return ship_array_color;
}

float Hive_Ship_Array::return_ship_array_damage()
{
	return ship_array_damage;
}

void Hive_Ship_Array::remove_swarm_engagement_target()
{
	ship_array_engagement_target = NULL;
}

void Hive_Ship_Array::reset_all_ships_in_swarm()
{
	for (int i = 0; i < Ship_Object_Array.size(); i++)
	{
		Ship_Object* ship = Ship_Object_Array[i];

		if (ship != NULL && ship->is_active())
		{
			ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
			ship->set_new_orbit(ship->return_home_orbit());
			
			ship->set_ship_combat_focus(NULL);

			if (ship->is_carrying_pod() == true)
			{
				ship->set_is_carrying_pod(false);
				ship->return_current_hive_pod_focus()->set_pod_is_being_plundered(false);
			}
		}
	}
}

void Hive_Ship_Array::set_active(bool _active)
{
	active = _active;
}

void Hive_Ship_Array::set_array_state(int new_array_state)
{
	swarm_array_state = new_array_state;

	reset_all_ships_in_swarm();
}

void Hive_Ship_Array::set_ship_array_damage(float damage)
{
	ship_array_damage = damage;
}

void Hive_Ship_Array::set_swarm_engaged_ship_array(Hive_Ship_Array * hive_ship_array)
{
	ship_array_engagement_target = hive_ship_array;
}

void Hive_Ship_Array::set_hive_engagement_target(Hive_Object * hive_target)
{
	hive_engagement_target = hive_target;
}

void Hive_Ship_Array::update()
{
	switch (swarm_array_state)
	{
	case SWARM_STATE_IDLE:
		break;
	case SWARM_STATE_COMBAT_ENGAGED:
		if (hive_engagement_target != NULL)
		{	
			if (hive_engagement_target->return_num_defending_swarms() == 0)
			{
				set_array_state(SWARM_STATE_PLUNDERING);
			}
			else if (return_engaged_ship_array() == NULL)
			{
				set_swarm_engaged_ship_array(hive_engagement_target->return_strongest_defending_ship_array());
			}
			else if (return_engaged_ship_array()->return_num_ships_in_swarm() == 0)
			{
				set_swarm_engaged_ship_array(NULL);
			}
		}
		break;
	case SWARM_STATE_PLUNDERING:
		if (hive_engagement_target->return_num_current_pods() <= 0)
		{
			set_array_state(SWARM_STATE_IDLE);
		}
		else if (hive_engagement_target->return_num_defending_swarms() > 0)
		{
			set_array_state(SWARM_STATE_COMBAT_ENGAGED);
		}
		break;
	case SWARM_STATE_DEFENDING:
		if (ship_array_engagement_target == NULL || ship_array_engagement_target->return_num_ships_in_swarm() <= 0)
		{
			remove_swarm_engagement_target();
			set_array_state(SWARM_STATE_IDLE);
		}
		break;
	}

	if (return_num_ships_in_swarm() <= 0)
	{
		swarm_array_state = SWARM_STATE_IDLE;
	}

	update_ships_in_swarm();

}

mat4* Hive_Ship_Array::update_all_ships_model_matrices()
{
	int ship_index = 0;

	for (int i = 0; i < Ship_Object_Array.size(); i++)
	{
		Ship_Object* new_ship = Ship_Object_Array[i];
		if (new_ship != NULL && new_ship->is_active())
		{
			ship_model_matrices[ship_index] = new_ship->return_current_model_matrix();
			new_ship->set_matrix_array_id(ship_index);
			ship_index++;
		}
	}

	return ship_model_matrices;
}

void Hive_Ship_Array::update_single_ship_model_matrix(Ship_Object* ship)
{
	ship_model_matrices[ship->return_matrix_array_id()] = ship->return_current_model_matrix();
}

void Hive_Ship_Array::update_ships_color_matrices()
{
	int ship_index = 0;

	for (int i = 0; i < Ship_Object_Array.size(); i++)
	{
		Ship_Object* new_ship = Ship_Object_Array[i];
		if (new_ship != NULL && new_ship->is_active())
		{
			ship_color_matrices[ship_index] = ship_array_color;
			ship_index++;
		}
	}
}

void Hive_Ship_Array::update_ships_in_swarm()
{
	for (int i = 0; i < Ship_Object_Array.size(); i++)
	{
		Ship_Object* ship = Ship_Object_Array[i];

		if (ship != NULL)
		{
			switch (swarm_array_state)
			{
			case SWARM_STATE_IDLE:
				update_ship_idle(ship);
				break;
			case SWARM_STATE_COMBAT_ENGAGED:
				if (ship_array_engagement_target != NULL) update_ship_engaged(ship);
				break;
			case SWARM_STATE_PLUNDERING:
				if (hive_engagement_target != NULL && hive_engagement_target->return_num_current_pods() > 0) update_ship_plundering(ship);
				break;
			case SWARM_STATE_DEFENDING:
				if (ship_array_engagement_target != NULL) update_ship_engaged(ship);
				break;
			}

			ship->process_ship_damage();

			ship->move();

			update_single_ship_model_matrix(ship);

			if (ship->return_ship_health() <= 0.0)
			{
				remove_ship_from_array(ship);
			}
		}
	}
}

void Hive_Ship_Array::update_ship_idle(Ship_Object* ship)
{
	ship->set_new_orbit(ship->return_home_orbit());
	ship->set_ship_combat_focus(NULL);
}

void Hive_Ship_Array::update_ship_engaged(Ship_Object* ship)
{
	switch (ship->return_current_state())
	{
	case Ship_Object::SHIP_STATE_IDLE:
		{
			Ship_Object* ship_target = ship_array_engagement_target->return_random_active_ship();

			if (ship_target != NULL)
			{
				ship->set_current_state(Ship_Object::SHIP_STATE_COMBAT_ENGAGED);
				ship->set_ship_combat_focus(ship_target);
				ship->return_ship_combat_focus()->add_engaged_ship(ship);

				ship->set_current_orbit_around_ship(ship->return_ship_combat_focus());

				//IF THE NEW ENGAGEMENT TARGET ISN'T ALREADY ENGAGED, RECIPROCALLY ENGAGE IT 
				if (ship_target->return_current_state() == Ship_Object::SHIP_STATE_IDLE)
				{
					ship_target->set_ship_combat_focus(ship);
					ship->add_engaged_ship(ship_target);
					ship_target->set_current_state(Ship_Object::SHIP_STATE_COMBAT_ENGAGED);
					ship->return_ship_combat_focus()->set_current_orbit_around_ship(ship);
				}
			}
		}
		break;
	case Ship_Object::SHIP_STATE_COMBAT_ENGAGED:
		if (ship->return_ship_combat_focus() == NULL || ship->return_ship_combat_focus()->return_ship_health() <= 0.0)
		{
			ship->remove_ship_focus();
			ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
			ship->set_new_orbit(ship->return_home_orbit());
		}
		else if (ship->return_ship_combat_focus() != NULL && ship->return_ship_combat_focus()->return_ship_health() > 0.0)
		{
			if (ship->return_ship_fire_cooldown() == 0.0 && distance(ship->return_current_transform_vector(), ship->return_ship_combat_focus()->return_current_transform_vector()) < 2 * ship->return_orbit_distance())
			{
				service_locator->return_gameplay_manager()->Create_New_Projectile(ship->return_current_transform_vector(), ship->return_ship_combat_focus()->return_current_transform_vector() + 4.0f*ship->return_ship_combat_focus()->return_ship_directional_vector());
				ship->set_ship_fire_cooldown(100.0);	
			}
			else ship->decrement_ship_fire_cooldown();

			ship->set_current_orbit_around_ship(ship->return_ship_combat_focus());
		}
		break;
	case Ship_Object::SHIP_STATE_CARRYING_POD:
		ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
		break;
	}
}

void Hive_Ship_Array::update_ship_plundering(Ship_Object* ship)
{
	if (ship->return_current_state() != 0 && ship->return_current_state() != 2 && ship->return_current_state() != 3) ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);

	switch (ship->return_current_state())
	{
	case Ship_Object::SHIP_STATE_IDLE:
		{
			if (hive_engagement_target->return_num_current_pods_not_currently_being_plundered() > 0)
			{
				Hive_Pod_Object * pod_to_plunder = hive_engagement_target->return_random_active_pod();

				if (pod_to_plunder != NULL)
				{
					ship->set_new_orbit(pod_to_plunder->return_world_translation_vector());
					ship->set_hive_pod_plunder_target(pod_to_plunder);
					ship->set_current_state(Ship_Object::SHIP_STATE_MOVING_TO_POD);
				}
			}
			else
			{
				ship->set_new_orbit(ship->return_home_orbit());
			}
		}
		break;
	case Ship_Object::SHIP_STATE_MOVING_TO_POD:
		if (ship->return_current_hive_pod_focus() != NULL && ship->return_current_hive_pod_focus()->return_pod_is_being_plundered() == false)
		{
			vec3 ship_transform = ship->return_current_transform_vector();
			vec3 hive_pod_transform = ship->return_current_hive_pod_focus()->return_world_translation_vector();

			float target_distance = distance(ship_transform, hive_pod_transform);

			if (target_distance < ship->return_orbit_distance() + 2 && ship->return_current_hive_pod_focus()->return_pod_is_being_plundered() == false)
			{
				ship->set_is_carrying_pod(true);
				ship->set_new_orbit(ship->return_home_orbit());
				ship->return_current_hive_pod_focus()->set_pod_is_being_plundered(true);
				ship->set_current_state(Ship_Object::SHIP_STATE_CARRYING_POD);
			}
		}
		else ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
		break;
	case Ship_Object::SHIP_STATE_CARRYING_POD:
		if (ship->return_current_hive_pod_focus() != NULL)
		{
			vec3 ship_transform = ship->return_current_transform_vector();
			vec3 hive_pod_transform = ship->return_current_hive_pod_focus()->return_world_translation_vector();

			float target_distance = distance(ship_transform, hive_pod_transform);

			ship->return_current_hive_pod_focus()->update_translation({ 0.0f,0.0f,0.0f }, ship->return_current_transform_vector());

			hive_engagement_target->update_single_hive_pod_model_matrix(ship->return_current_hive_pod_focus());

			if (distance(ship->return_current_transform_vector(), ship->return_home_orbit()) < ship->return_orbit_distance() + 2)
			{
				increment_num_pods_to_add_to_hive(ship->return_current_hive_pod_focus()->return_pod_color());
				hive_engagement_target->remove_hive_pod(ship->return_current_hive_pod_focus());
				ship->remove_hive_pod_plunder_target();
				ship->set_is_carrying_pod(false);
				ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
			}
		}
		else
		{
			cout << "this shouldn't happen" << endl;
			ship->set_current_state(Ship_Object::SHIP_STATE_IDLE);
		}
		break;
	}

}
