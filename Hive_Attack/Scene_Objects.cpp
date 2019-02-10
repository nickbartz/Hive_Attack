#include<Scene_Graph.h>
#include<Scene_Objects.h>
#include<vector>

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

using namespace std;
using namespace glm;

// HIVE SHIP

void Ship_Object::ship_init(glm::vec3 start_location, vec3 _orbit_location)
{
	//loadOBJ_no_uvs("Basic_Ship.obj", vertices, uvs, normals);

	//indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	update_transform_matrix(start_location);

	orbit_location = _orbit_location;
	home_orbit_location = _orbit_location;
}

bool Ship_Object::is_active()
{
	return active;
}

bool Ship_Object::is_engaged()
{
	return engaged;
}

void Ship_Object::set_engaged(bool _engaged)
{
	engaged = _engaged;
}

int Ship_Object::get_major_array_id()
{
	return major_array_id;
}

void Ship_Object::set_major_array_id(int _major_array_id)
{
	major_array_id = _major_array_id;
}

void Ship_Object::set_active(bool _active)
{
	active = _active;
}

void Ship_Object::update_rotation_matrix(glm::vec3 euler_rotation_vector)
{
	current_rotation_vector = euler_rotation_vector;
	glm::quat MyQuaternion;
	glm::vec3 EulerAngles(current_rotation_vector);
	MyQuaternion = glm::quat(EulerAngles);
	RotationMatrix = glm::toMat4(MyQuaternion);
}

void Ship_Object::increment_rotation_matrix(vec3 incremental_rotation_vector)
{
	current_rotation_vector += incremental_rotation_vector;
	update_rotation_matrix(current_rotation_vector);
}

void Ship_Object::update_transform_matrix(glm::vec3 transform_vector)
{
	Transform_Matrix = glm::translate(transform_vector);
	current_transform_vector = transform_vector;
}

void Ship_Object::increment_transform_matrix(glm::vec3 current_direction_vector)
{
	glm::vec3 new_transform_vector = current_transform_vector + current_direction_vector;
	current_transform_vector = new_transform_vector;
	update_transform_matrix(current_transform_vector);
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
	return Transform_Matrix*RotationMatrix*ScaleMatrix;
}

vec3 Ship_Object::return_home_orbit()
{
	return home_orbit_location;
}

void Ship_Object::rotate_directional_vector(float angle)
{
	float x_coord = ((current_direction_vector.x) * cos(angle)) - ((current_direction_vector.z)*sin(angle));
	float z_coord = ((current_direction_vector.z) * cos(angle)) + ((current_direction_vector.x)*sin(angle));

	current_direction_vector.x = x_coord;
	current_direction_vector.z = z_coord;
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

void Ship_Object::move()
{
	if (current_engagement_focus != NULL)
	{
		orbit_location = current_engagement_focus->current_transform_vector;
	}

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
		
		float grav_accel = speed;

		float body_vel_x = (sin(angle) * grav_accel);
		float body_vel_y = (cos(angle) * grav_accel);
	
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

	increment_transform_matrix(current_direction_vector);

	float rotation_angle = atan2f(current_direction_vector.z, current_direction_vector.x);

	update_rotation_matrix({ rotation_angle, 0.0,-1.55 });
}

void Ship_Object::check_nearby_targets()
{

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

		engaged = true;
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

void Ship_Object::Reduce_Ship_Health(float health_decrement)
{
	if (ship_health > 0.0)
	{
		ship_health -= health_decrement;
	}
}

float Ship_Object::get_health()
{
	return ship_health;
}

float Ship_Object::return_ship_damage()
{
	return ship_damage;
}

void Ship_Object::set_ship_damage(float _ship_damage)
{
	ship_damage = _ship_damage;
}

void Ship_Object::add_engaged_ship(Ship_Object * ship_object)
{
	ships_engaging.push_back(ship_object);
}

void Ship_Object::remove_engaged_ship(Ship_Object* ship_object)
{
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		if (ships_engaging[i] == ship_object) ships_engaging.erase(ships_engaging.begin() + i);
	}
}

void Ship_Object::double_check_engaged_ships()
{
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		if (ships_engaging[i]->is_active() == false) ships_engaging.erase(ships_engaging.begin() + i);
	}
}

float Ship_Object::calculate_damage_from_engaging_ships()
{
	float damage = 0.0;
	for (int i = 0; i < ships_engaging.size(); i++)
	{
		damage += ships_engaging[i]->return_ship_damage();
	}

	return damage;
}

// HIVE SHIP ARRAY MANIFEST 

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


// HIVE SHIP ARRAY

Hive_Ship_Array::Hive_Ship_Array()
{
	uniq_id = rand() % 1000;
}

void Hive_Ship_Array::init_hive_ship_array(vec3 ship_color, float ship_damage)
{
	ship_array_color = ship_color;
	ship_array_damage = ship_damage;
}

Ship_Object* Hive_Ship_Array::add_ship_to_array(vec3 current_location, vec3 orbit_location)
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Ship_Object_Array[i].is_active() == false)
		{
			Ship_Object new_ship;
			new_ship.current_transform_vector = current_location;
			new_ship.orbit_location = orbit_location;
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

int Hive_Ship_Array::array_begin()
{
	return 0;
}

int Hive_Ship_Array::array_end()
{
	return max_list_pointer - 1;
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

void Hive_Ship_Array::move_ships()
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		Ship_Object_Array[i].move();
	}
}

bool Hive_Ship_Array::is_engaged()
{
	return engaged;
}

void Hive_Ship_Array::set_engagement_target(Hive_Ship_Array* ship_array)
{
	engagement_target = ship_array;
}

void Hive_Ship_Array::set_ship_array_damage(float damage)
{
	ship_array_damage = damage;
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

void Hive_Ship_Array::set_engaged(bool _engaged)
{
	engaged = _engaged;
}

Hive_Ship_Array* Hive_Ship_Array::return_engaged_ship_array()
{
	return engagement_target;
}

// HIVE POD

void Hive_Pod_Object::Init_Hive_Pod(glm::vec3 _translation_vector, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset)
{
	octohedron_coordinates = base_grid_coords + face_grid_offset;

	translation_vector = _translation_vector;

	init_squares(translation_vector);
	init_hexagons(translation_vector);
}

void Hive_Pod_Object::init_squares(vec3 translation_vector)
{
	Square new_square;
	new_square.init_square(octo_vertices["v1324"], octo_vertices["v2314"], octo_vertices["v2413"], octo_vertices["v1423"], translation_vector); // A+ (grid x + 2)
	new_square.set_tessalation_vector(octo_vertices["v1324"] - octo_vertices["v3142"]);
	new_square.opposite_square = 1;
	new_square.face_grid_offset = { 2,0,0 };
	square_array[0] = (new_square);

	new_square.init_square(octo_vertices["v3142"], octo_vertices["v3241"], octo_vertices["v4231"], octo_vertices["v4132"], translation_vector); // A+ (grid x - 2)
	new_square.set_tessalation_vector(octo_vertices["v3142"] - octo_vertices["v1324"]);
	new_square.opposite_square = 0;
	new_square.face_grid_offset = { -2,0,0 };
	square_array[1] = (new_square);

	new_square.init_square(octo_vertices["v3124"], octo_vertices["v4123"], octo_vertices["v4213"], octo_vertices["v3214"], translation_vector); // C ( grid z + 2)
	new_square.set_tessalation_vector(octo_vertices["v3124"] - octo_vertices["v1342"]);
	new_square.opposite_square = 3;
	new_square.face_grid_offset = { 0,0,2 };
	square_array[2] = (new_square);

	new_square.init_square(octo_vertices["v1432"], octo_vertices["v2431"], octo_vertices["v2341"], octo_vertices["v1342"], translation_vector); // C ( grid z -2)
	new_square.set_tessalation_vector(octo_vertices["v1432"] - octo_vertices["v3214"]);
	new_square.opposite_square = 2;
	new_square.face_grid_offset = { 0,0,-2 };
	square_array[3] = (new_square);

	new_square.init_square(octo_vertices["v2143"], octo_vertices["v2134"], octo_vertices["v1234"], octo_vertices["v1243"], translation_vector); // B ( y - 2)
	new_square.set_tessalation_vector(octo_vertices["v2143"] - octo_vertices["v4321"]);
	new_square.opposite_square = 5;
	new_square.face_grid_offset = { 0,-2,0 };
	square_array[4] = (new_square);

	new_square.init_square(octo_vertices["v3412"], octo_vertices["v4312"], octo_vertices["v4321"], octo_vertices["v3421"], translation_vector); // B ( y + 2)
	new_square.set_tessalation_vector(octo_vertices["v3412"] - octo_vertices["v1234"]);
	new_square.opposite_square = 4;
	new_square.face_grid_offset = { 0,2,0 };
	square_array[5] = (new_square);
}

void Hive_Pod_Object::init_hexagons(vec3 translation_vector)
{
	Hexagon new_hexagon;
	new_hexagon.init_hexagon(octo_vertices["v2314"], octo_vertices["v3214"], octo_vertices["v4213"], octo_vertices["v4312"], octo_vertices["v3412"], octo_vertices["v2413"], translation_vector); // C ( x + 1, z + 1, y + 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v2314"] - octo_vertices["v1243"]);
	new_hexagon.opposite_hexagon = 6;
	new_hexagon.face_grid_offset = { 1,1,1 };
	hexagon_array[0] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v4213"], octo_vertices["v4123"], octo_vertices["v4132"], octo_vertices["v4231"], octo_vertices["v4321"],octo_vertices["v4312"], translation_vector); // D ( x - 1, z + 1, y + 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v4213"] - octo_vertices["v1324"]);
	new_hexagon.opposite_hexagon = 5;
	new_hexagon.face_grid_offset = { -1,1,1 };
	hexagon_array[1] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v2134"], octo_vertices["v3124"], octo_vertices["v3214"], octo_vertices["v2314"], octo_vertices["v1324"], octo_vertices["v1234"], translation_vector); // B ( x + 1, z + 1, y-1)
	new_hexagon.set_tessalation_vector(octo_vertices["v2134"] - octo_vertices["v3241"]);
	new_hexagon.opposite_hexagon = 7;
	new_hexagon.face_grid_offset = { 1,-1,1 };
	hexagon_array[2] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v4123"], octo_vertices["v3124"], octo_vertices["v2134"], octo_vertices["v2143"], octo_vertices["v3142"], octo_vertices["v4132"], translation_vector); // A ( x- 1, z+1, y-1)
	new_hexagon.set_tessalation_vector(octo_vertices["v4123"] - octo_vertices["v3412"]);
	new_hexagon.opposite_hexagon = 4;
	new_hexagon.face_grid_offset = { -1,-1,1 };
	hexagon_array[3] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v2413"], octo_vertices["v3412"], octo_vertices["v3421"], octo_vertices["v2431"], octo_vertices["v1432"], octo_vertices["v1423"], translation_vector); // A ( x + 1, z - 1, y + 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v2413"] - octo_vertices["v3124"]);
	new_hexagon.opposite_hexagon = 3;
	new_hexagon.face_grid_offset = { 1,1,-1 };
	hexagon_array[4] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v1243"],octo_vertices["v1234"], octo_vertices["v1324"], octo_vertices["v1423"], octo_vertices["v1432"], octo_vertices["v1342"], translation_vector); // D ( x + 1, z - 1, y - 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v1243"] - octo_vertices["v4132"]);
	new_hexagon.opposite_hexagon = 1;
	new_hexagon.face_grid_offset = { 1,-1,-1 };
	hexagon_array[5] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v3142"], octo_vertices["v2143"], octo_vertices["v1243"], octo_vertices["v1342"], octo_vertices["v2341"], octo_vertices["v3241"], translation_vector); // C ( x - 1, z - 1, y - 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v3142"] - octo_vertices["v4213"]);
	new_hexagon.opposite_hexagon = 0;
	new_hexagon.face_grid_offset = { -1,-1,-1 };
	hexagon_array[6] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v3421"], octo_vertices["v4321"], octo_vertices["v4231"], octo_vertices["v3241"], octo_vertices["v2341"], octo_vertices["v2431"], translation_vector); // B ( x - 1, z - 1, y+1)
	new_hexagon.set_tessalation_vector(octo_vertices["v3421"] - octo_vertices["v2314"]);
	new_hexagon.opposite_hexagon = 2;
	new_hexagon.face_grid_offset = { -1,1,-1 };
	hexagon_array[7] = (new_hexagon);
}

vec3 Hive_Pod_Object::get_translation_vector()
{
	return translation_vector;
}

vec3 Hive_Pod_Object::get_scaled_translation_vector()
{
	return { translation_vector.x*scale_vector.x, translation_vector.y*scale_vector.y, translation_vector.z*scale_vector.z };
}

void Hive_Pod_Object::add_triangle_array_components(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
{
	for (int i = 0; i < 6; i++)
	{
		if (square_array[i].obscured == false)
		{
			square_array[i].return_square_triangles(vertices, uvs, normals);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (hexagon_array[i].obscured == false)
		{
			hexagon_array[i].return_hexagon_triangles(vertices, uvs, normals);
		}
	}
}

void Hive_Pod_Object::check_for_total_obscurity()
{
	all_squares_obscured = true;
	// Conscious decision to limit square extrustion to the 4 squares on the sides (0-3), not the north and south facing squares
	for (int i = 0; i< 4; i++)
	{
		if (square_array[i].obscured == false) all_squares_obscured = false;
	}

	all_hexagons_obscured = true;
	for (int i = 0; i< 8; i++)
	{
		if (hexagon_array[i].obscured == false) all_hexagons_obscured = false;
	}
}

void Hive_Pod_Object::update_translation(vec3 translation_vector)
{
	hive_ship_pointer->orbit_location = translation_vector;
	hive_ship_pointer->set_home_orbit(translation_vector);
	hive_ship_pointer->current_transform_vector = translation_vector;
}

void Hive_Pod_Object::register_hive_ship(Ship_Object* _hive_ship)
{
	hive_ship_pointer = _hive_ship;
}

// HIVE OBJECT

Hive_Object::Hive_Object()
{

}

void Hive_Object::Init_Hive_Object(vec3 _Hive_Color, float hive_damage)
{
	glm::quat MyQuaternion;
	glm::vec3 EulerAngles({ 0.0f,0.0f,0.0f });
	MyQuaternion = glm::quat(EulerAngles);
	RotationMatrix = glm::toMat4(MyQuaternion);
	Hive_Color = _Hive_Color;
	hive_ship_array.init_hive_ship_array(Hive_Color, hive_damage);

	register_new_hive_pod({ 0.0f, 0.0f,0.0f }, { 0,0,0 }, { 0,0,0 });

	update_model_vertices();

	loadOBJ_no_uvs("Basic_Ship.obj", ship_vertices, ship_uvs, ship_normals);
	indexVBO(ship_vertices, ship_uvs, ship_normals, ship_indices, ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals);
}

void Hive_Object::update_translation_matrix(vec3 translation)
{
	TranslationMatrix = translate(translation);

	for (int i = 0; i < num_current_cells; i++)
	{
		cell_array[i].update_translation(cell_array[i].get_translation_vector() + translation);
	}
}

void Hive_Object::register_new_hive_pod(vec3 translation_vector, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset)
{
	Hive_Pod_Object new_octo;
	new_octo.Init_Hive_Pod(translation_vector, base_grid_coords, face_grid_offset);
	Grid_Coord new_grid_coords = new_octo.octohedron_coordinates;

	cell_array[num_current_cells] = new_octo;
	array_map[new_grid_coords] = &cell_array[num_current_cells];
	register_new_hive_ship(&cell_array[num_current_cells], translation_vector, translation_vector);
	Manage_Obscurity(&cell_array[num_current_cells]);
	num_current_cells++;
}

void Hive_Object::register_new_hive_ship(Hive_Pod_Object* hive_pod_pointer, vec3 translation_vector, vec3 orbit_center)
{
	hive_pod_pointer->hive_ship_pointer = hive_ship_array.add_ship_to_array(translation_vector, orbit_center);
}

void Hive_Object::extrude_new_octo()
{
	int array_index;
	bool square_found = false;
	int face_num = 0;

	bool extrusion_found = false;

	// Try to find a cell where not all squares are obscured
	array_index = rand() % num_current_cells;
	for (int i = 0; i < num_current_cells; i++)
	{
		if (cell_array[array_index].all_squares_obscured == false)
		{
			extrude_from_square(array_index);
			return;
		}
		array_index++;
		if (array_index > num_current_cells - 1) array_index = 0;
	}

}

void Hive_Object::extrude_from_square(int array_index)
{
	glm::vec3 translation_vector;
	int acceptable_faces[4] = { 0,1,2,3 };
	int face_index = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		if (cell_array[array_index].square_array[acceptable_faces[face_index]].return_obscured() == false)
		{
			translation_vector = cell_array[array_index].get_translation_vector() + cell_array[array_index].square_array[acceptable_faces[face_index]].tessallation_vector;

			register_new_hive_pod(translation_vector, cell_array[array_index].octohedron_coordinates, cell_array[array_index].square_array[acceptable_faces[face_index]].face_grid_offset);

			return;
		}

		face_index++;
		if (face_index > 3) face_index = 0;
	}

	cout << "could not find square to extrude from - this should be impossible" << endl;
}

void Hive_Object::extrude_from_hexagon(int array_index)
{
	// Find the first hexagon that isn't obscured
	int face_num = rand() % 8;
	glm::vec3 translation_vector;
	for (int i = 0; i < 8; i++)
	{
		if (cell_array[array_index].hexagon_array[face_num].return_obscured() == false)
		{
			translation_vector = cell_array[array_index].get_translation_vector() + cell_array[array_index].hexagon_array[face_num].tessallation_vector;

			register_new_hive_pod(translation_vector, cell_array[array_index].octohedron_coordinates, cell_array[array_index].hexagon_array[face_num].face_grid_offset);

			return;
		}

		face_num++;
		if (face_num > 7) face_num = 0;
	}
}

void Hive_Object::Manage_Obscurity(Hive_Pod_Object* octo)
{
	for (int i = 0; i < 6; i++)
	{
		int opposite_face_index = octo->square_array[i].opposite_square;
		Grid_Coord square_adjacent_coord;

		square_adjacent_coord.grid_x = octo->octohedron_coordinates.grid_x + octo->square_array[i].face_grid_offset.grid_x;
		square_adjacent_coord.grid_y = octo->octohedron_coordinates.grid_y + octo->square_array[i].face_grid_offset.grid_y;
		square_adjacent_coord.grid_z = octo->octohedron_coordinates.grid_z + octo->square_array[i].face_grid_offset.grid_z;

		if (array_map.find(square_adjacent_coord) != array_map.end())
		{
			octo->square_array[i].set_obscured(true);
			octo->check_for_total_obscurity();
			array_map[square_adjacent_coord]->square_array[opposite_face_index].set_obscured(true);
			array_map[square_adjacent_coord]->check_for_total_obscurity();

		}
	}

	for (int i = 0; i < 8; i++)
	{
		int opposite_face_index = octo->hexagon_array[i].opposite_hexagon;
		Grid_Coord hexagon_adjacent_coord;
		hexagon_adjacent_coord.grid_x = octo->octohedron_coordinates.grid_x + octo->hexagon_array[i].face_grid_offset.grid_x;
		hexagon_adjacent_coord.grid_y = octo->octohedron_coordinates.grid_y + octo->hexagon_array[i].face_grid_offset.grid_y;
		hexagon_adjacent_coord.grid_z = octo->octohedron_coordinates.grid_z + octo->hexagon_array[i].face_grid_offset.grid_z;

		if (array_map.find(hexagon_adjacent_coord) != array_map.end())
		{
			octo->hexagon_array[i].set_obscured(true);
			octo->check_for_total_obscurity();
			array_map[hexagon_adjacent_coord]->hexagon_array[opposite_face_index].set_obscured(true);
			array_map[hexagon_adjacent_coord]->check_for_total_obscurity();
		}
	}
}

void Hive_Object::update_model_vertices()
{
	vertices.clear();
	uvs.clear();
	normals.clear();

	for (int i = 0; i < num_current_cells; i++)
	{
		cell_array[i].add_triangle_array_components(vertices, uvs, normals);
	}

	indexed_vertices.clear();
	indexed_uvs.clear();
	indexed_normals.clear();
	indices.clear();

	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
}

void Hive_Object::load_buffer_return_specs()
{
	loaded_specs = load_object_to_buffers(indexed_vertices, indexed_uvs, indexed_normals, indices);

	loaded_ship_specs = load_object_to_buffers(ship_indexed_vertices, ship_indexed_uvs, ship_indexed_normals, ship_indices);
}

vec3 Hive_Object::return_hive_color()
{
	return Hive_Color;
}

mat4 Hive_Object::return_model_matrix()
{
	return TranslationMatrix*RotationMatrix*ScaleMatrix;
}
