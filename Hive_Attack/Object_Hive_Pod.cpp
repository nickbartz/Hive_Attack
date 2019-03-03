#include<Object_Hive_Pod.h>
#include<Object_Hive_Ship.h>

void Hive_Pod_Object::destroy()
{
	pod_is_active = false;
}

void Hive_Pod_Object::Init_Hive_Pod(Service_Locator* _service_locator, vec3 local_translation, vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset, vec3 _pod_color)
{
	service_locator = _service_locator;

	base_octohedron = new Octohedron_Model;
	base_octohedron->init_base_octohedron();

	octohedron_coordinates = base_grid_coords + face_grid_offset;

	update_translation(local_translation, world_translation);

	pod_is_active = true;

	pod_color = _pod_color;

	max_ship_build_counter = 10000;
	max_num_pod_ships = 2;
}

bool Hive_Pod_Object::is_active()
{
	return pod_is_active;
}

bool Hive_Pod_Object::is_able_to_create_ship()
{
	if (ship_build_counter == 0 && hive_ship_array.size() < max_num_pod_ships)
	{
		ship_build_counter = max_ship_build_counter;
		return true;
	}
	else return false;
}

vec3 Hive_Pod_Object::return_local_translation_vector()
{
	return local_translation_vector;
}

vec3 Hive_Pod_Object::return_world_translation_vector()
{
	return world_translation_vector;
}

vec3 Hive_Pod_Object::return_scaled_translation_vector()
{
	return { local_translation_vector.x*scale_vector.x, local_translation_vector.y*scale_vector.y, local_translation_vector.z*scale_vector.z };
}

void Hive_Pod_Object::remove_associated_ship_from_array(Ship_Object * ship)
{
	for (int i = 0; i < hive_ship_array.size(); i++)
	{
		if (hive_ship_array[i] == ship)
		{
			hive_ship_array.erase(hive_ship_array.begin() + i);
			break;
		}
	}
}

Octohedron_Model * Hive_Pod_Object::return_octohedron_base()
{
	return base_octohedron;
}

void Hive_Pod_Object::register_hive_ship(Ship_Object* _hive_ship)
{
	_hive_ship->set_hive_pod_owner(this);

	hive_ship_array.push_back(_hive_ship);
}

float Hive_Pod_Object::return_hive_pod_health()
{
	return hive_pod_health;
}

int Hive_Pod_Object::return_current_matrix_pointer()
{
	return matrices_array_pointer;
}

vec3 Hive_Pod_Object::return_pod_color()
{
	return pod_color;
}

mat4 Hive_Pod_Object::return_current_model_matrix()
{
	mat4 model_matrix = translate(vec3{ 0.0f,-1.0f,0.0f })*Transform_Matrix * RotationMatrix * ScaleMatrix;

	return model_matrix;
}

bool Hive_Pod_Object::return_pod_is_being_plundered()
{
	return pod_is_being_plundered;
}

void Hive_Pod_Object::set_active(bool active)
{
	pod_is_active = active;
}

void Hive_Pod_Object::set_pod_is_being_plundered(bool is_being_plundered)
{
	pod_is_being_plundered = is_being_plundered;
}

void Hive_Pod_Object::set_pod_color(vec3 _pod_color)
{
	pod_color = _pod_color;
}

void Hive_Pod_Object::set_current_matrix_pointer(int array_pointer)
{
	matrices_array_pointer = array_pointer;
}

void Hive_Pod_Object::take_damage(float damage)
{
	hive_pod_health -= damage;

	if (hive_pod_health <= 0.0f) hive_pod_health = 0.0f;
}

void Hive_Pod_Object::update_translation(vec3 _local_translation_vector, vec3 _world_translation_vector)
{
	local_translation_vector = _local_translation_vector;
	world_translation_vector = _world_translation_vector;

	Transform_Matrix = translate(_world_translation_vector);
}

void Hive_Pod_Object::update()
{
	ship_build_counter--;

	if (ship_build_counter <= 0) ship_build_counter = 0;
}