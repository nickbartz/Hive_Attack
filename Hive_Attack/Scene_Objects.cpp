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
#include<Ship_Array.h>

using namespace std;
using namespace glm;


void Octohedron_Model::init_base_octohedron()
{
	octo_vertices["v1234"] = { d, 0, 0 }; ;
	octo_vertices["v1243"] = { 4 / d,	2 / d,	1 };
	octo_vertices["v1324"] = { 4 / d, -3 / d,	0 };
	octo_vertices["v1342"] = { 2 / d,	1 / d,	2 };
	octo_vertices["v1423"] = { 2 / d, -4 / d, 1 };
	octo_vertices["v1432"] = { 1 / d, -2 / d, 2 };

	octo_vertices["v2134"] = { 4 / d, 2 / d, -1 };
	octo_vertices["v2143"] = { 3 / d, 4 / d, 0 };
	octo_vertices["v2314"] = { 2 / d,-4 / d,-1 };
	octo_vertices["v2341"] = { -1 / d,2 / d,2 };
	octo_vertices["v2431"] = { -2 / d,-1 / d,2 };
	octo_vertices["v2413"] = { 0,-d,0 };

	octo_vertices["v3142"] = -octo_vertices["v2413"];
	octo_vertices["v3124"] = -octo_vertices["v2431"];
	octo_vertices["v3241"] = -octo_vertices["v2314"];
	octo_vertices["v3214"] = -octo_vertices["v2341"];
	octo_vertices["v3412"] = -octo_vertices["v2143"];
	octo_vertices["v3421"] = -octo_vertices["v2134"];

	octo_vertices["v4123"] = -octo_vertices["v1432"];
	octo_vertices["v4132"] = -octo_vertices["v1423"];
	octo_vertices["v4231"] = -octo_vertices["v1324"];
	octo_vertices["v4213"] = -octo_vertices["v1342"];
	octo_vertices["v4312"] = -octo_vertices["v1243"];
	octo_vertices["v4321"] = -octo_vertices["v1234"];

	map<string, vec3>::iterator it;

	glm::quat MyQuaternion;
	glm::vec3 EulerAngles({ 0.0f,0.0f,1.1f });
	MyQuaternion = glm::quat(EulerAngles);

	for (it = octo_vertices.begin(); it != octo_vertices.end(); it++)
	{
		it->second *= 0.5;
		it->second = rotate(MyQuaternion, it->second);
	}

	init_squares({ 0.0f,0.0f,0.0f });
	init_hexagons({ 0.0f,0.0f,0.0f });

}

void Octohedron_Model::init_squares(vec3 translation_vector)
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

void Octohedron_Model::init_hexagons(vec3 translation_vector)
{
	Hexagon new_hexagon;
	new_hexagon.init_hexagon(octo_vertices["v2314"], octo_vertices["v3214"], octo_vertices["v4213"], octo_vertices["v4312"], octo_vertices["v3412"], octo_vertices["v2413"], translation_vector); // C ( x + 1, z + 1, y + 1)
	new_hexagon.set_tessalation_vector(octo_vertices["v2314"] - octo_vertices["v1243"]);
	new_hexagon.opposite_hexagon = 6;
	new_hexagon.face_grid_offset = { 1,1,1 };
	hexagon_array[0] = (new_hexagon);

	new_hexagon.init_hexagon(octo_vertices["v4213"], octo_vertices["v4123"], octo_vertices["v4132"], octo_vertices["v4231"], octo_vertices["v4321"], octo_vertices["v4312"], translation_vector); // D ( x - 1, z + 1, y + 1)
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

	new_hexagon.init_hexagon(octo_vertices["v1243"], octo_vertices["v1234"], octo_vertices["v1324"], octo_vertices["v1423"], octo_vertices["v1432"], octo_vertices["v1342"], translation_vector); // D ( x + 1, z - 1, y - 1)
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

vec3 Octohedron_Model::return_octo_vertex_vector(string vertex)
{
	return octo_vertices[vertex];
}

void Octohedron_Model::add_triangle_array_components(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
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

bool Octohedron_Model::check_for_total_obscurity()
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

	return all_squares_obscured;
}



// HIVE POD //

void Hive_Pod_Object::Init_Hive_Pod(vec3 local_translation, vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset)
{
	octohedron_coordinates = base_grid_coords + face_grid_offset;

	update_translation(local_translation, world_translation);

	pod_is_active = true;
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

Octohedron_Model * Hive_Pod_Object::return_octohedron_base()
{
	return base_octohedron;
}

void Hive_Pod_Object::update_translation(vec3 _local_translation_vector, vec3 _world_translation_vector)
{
	local_translation_vector = _local_translation_vector;
	world_translation_vector = _world_translation_vector;

	Transform_Matrix = translate(_world_translation_vector);
}

void Hive_Pod_Object::register_hive_ship(Ship_Object* _hive_ship)
{
	hive_ship_pointer = _hive_ship;

	hive_ship_pointer->orbit_location = local_translation_vector;
	hive_ship_pointer->set_home_orbit(local_translation_vector);
	hive_ship_pointer->current_transform_vector = local_translation_vector;
}

void Hive_Pod_Object::take_damage(float damage)
{
	hive_pod_health -= damage;

	if (hive_pod_health <= 0.0f) hive_pod_health = 0.0f;
}

float Hive_Pod_Object::return_hive_pod_health()
{
	return hive_pod_health;
}

bool Hive_Pod_Object::is_active()
{
	return pod_is_active;
}

void Hive_Pod_Object::set_active(bool active)
{
	pod_is_active = active;
}

void Hive_Pod_Object::set_major_array_id(int array_id)
{
	major_array_id = array_id;
}

int Hive_Pod_Object::return_major_array_id()
{
	return major_array_id;
}

mat4 Hive_Pod_Object::return_current_model_matrix()
{
	mat4 model_matrix = translate(vec3{0.0f,-1.0f,0.0f})*Transform_Matrix * RotationMatrix * ScaleMatrix;

	return model_matrix;
}

// HIVE OBJECT // 

Hive_Object::Hive_Object()
{

}

void Hive_Object::Init_Hive_Object(vec3 initial_location, vec3 _Hive_Color, float hive_damage)
{
	cout << "Creating New Hive Object" << endl;

	glm::quat MyQuaternion;
	glm::vec3 EulerAngles({ 0.0f,0.0f,0.0f });
	MyQuaternion = glm::quat(EulerAngles);
	RotationMatrix = glm::toMat4(MyQuaternion);

	Hive_Color = _Hive_Color;

	hive_ship_array.init_hive_ship_array(Hive_Color, hive_damage);

	hive_translation_vector = initial_location;

	register_new_hive_pod({ 0.0,0.0,0.0 }, hive_translation_vector, { 0,0,0 }, { 0,0,0 });

	update_model_vertices();
}

bool Hive_Object::check_engagement_target_fleet_destroyed()
{
	if (hive_engagement_target->is_fleet_destroyed()) return true;
	else return false;
}

void Hive_Object::extrude_new_octo()
{
	cout << "Extruding Hive Pod" << endl;

	int array_index = 0;
	bool square_found = false;
	int face_num = 0;

	bool extrusion_found = false;

	// Try to find a cell where not all squares are obscured
	array_index = rand() % num_current_hive_pods;

	for (int i = 0; i < num_current_hive_pods; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index].return_octohedron_base();

		if (base_octohedron->check_for_total_obscurity() == false)
		{
			/*if (rand() % 2 == 0) extrude_from_square(array_index);
			else 
			*/extrude_from_hexagon(array_index);
			return;
		}


		array_index++;
		if (array_index > num_current_hive_pods - 1) array_index = 0;
	}
}

void Hive_Object::extrude_from_square(int array_index)
{

	int acceptable_faces[4] = { 0,1,2,3 };
	int face_index = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index].return_octohedron_base();

		if (base_octohedron->square_array[acceptable_faces[face_index]].return_obscured() == false)
		{
			vec3 local_position = Hive_Pod_Array[array_index].return_local_translation_vector() + base_octohedron->square_array[acceptable_faces[face_index]].tessallation_vector;
			vec3 world_position = local_position + hive_translation_vector;

			register_new_hive_pod(local_position, world_position, Hive_Pod_Array[array_index].octohedron_coordinates, base_octohedron->square_array[acceptable_faces[face_index]].face_grid_offset);

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
	for (int i = 0; i < 8; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index].return_octohedron_base();
		if (base_octohedron->hexagon_array[face_num].return_obscured() == false)
		{
			vec3 local_position = Hive_Pod_Array[array_index].return_local_translation_vector() + base_octohedron->hexagon_array[face_num].tessallation_vector;
			vec3 world_position = local_position + hive_translation_vector;

			register_new_hive_pod(local_position, world_position, Hive_Pod_Array[array_index].octohedron_coordinates, base_octohedron->hexagon_array[face_num].face_grid_offset);

			return;
		}

		face_num++;
		if (face_num > 7) face_num = 0;
	}
}

bool Hive_Object::is_engaged()
{
	return hive_is_engaged;
}

bool Hive_Object::is_fleet_destroyed()
{
	if (hive_ship_array.return_num_ships_in_swarm() == 0) return true;
	else return false;
}

void Hive_Object::load_buffer_return_specs()
{
	loaded_specs = create_object_buffers(indexed_vertices, indexed_uvs, indexed_normals, indices);
}

void Hive_Object::Manage_Obscurity(Hive_Pod_Object* octo)
{
	Octohedron_Model* base_octohedron = octo->return_octohedron_base();

	for (int i = 0; i < 6; i++)
	{
		int opposite_face_index = base_octohedron->square_array[i].opposite_square;
		Grid_Coord square_adjacent_coord;

		square_adjacent_coord.grid_x = octo->octohedron_coordinates.grid_x + base_octohedron->square_array[i].face_grid_offset.grid_x;
		square_adjacent_coord.grid_y = octo->octohedron_coordinates.grid_y + base_octohedron->square_array[i].face_grid_offset.grid_y;
		square_adjacent_coord.grid_z = octo->octohedron_coordinates.grid_z + base_octohedron->square_array[i].face_grid_offset.grid_z;

		if (Pod_Grid_Map.find(square_adjacent_coord) != Pod_Grid_Map.end())
		{
			base_octohedron->square_array[i].set_obscured(true);
			base_octohedron->check_for_total_obscurity();
			Pod_Grid_Map[square_adjacent_coord]->return_octohedron_base()->square_array[opposite_face_index].set_obscured(true);
			Pod_Grid_Map[square_adjacent_coord]->return_octohedron_base()->check_for_total_obscurity();

		}
	}

	for (int i = 0; i < 8; i++)
	{
		int opposite_face_index = base_octohedron->hexagon_array[i].opposite_hexagon;
		Grid_Coord hexagon_adjacent_coord;
		hexagon_adjacent_coord.grid_x = octo->octohedron_coordinates.grid_x + base_octohedron->hexagon_array[i].face_grid_offset.grid_x;
		hexagon_adjacent_coord.grid_y = octo->octohedron_coordinates.grid_y + base_octohedron->hexagon_array[i].face_grid_offset.grid_y;
		hexagon_adjacent_coord.grid_z = octo->octohedron_coordinates.grid_z + base_octohedron->hexagon_array[i].face_grid_offset.grid_z;

		if (Pod_Grid_Map.find(hexagon_adjacent_coord) != Pod_Grid_Map.end())
		{
			base_octohedron->hexagon_array[i].set_obscured(true);
			base_octohedron->check_for_total_obscurity();
			Pod_Grid_Map[hexagon_adjacent_coord]->return_octohedron_base()->hexagon_array[opposite_face_index].set_obscured(true);
			Pod_Grid_Map[hexagon_adjacent_coord]->return_octohedron_base()->check_for_total_obscurity();
		}
	}
}

void Hive_Object::register_new_hive_pod(vec3 local_translation,vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset)
{
	Hive_Pod_Object new_octo;

	new_octo.Init_Hive_Pod(local_translation, world_translation, base_grid_coords, face_grid_offset);
	Grid_Coord new_grid_coords = new_octo.octohedron_coordinates;

	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Hive_Pod_Array[i].is_active() == false)
		{
			Hive_Pod_Array[i] = new_octo;
			Hive_Pod_Array[i].set_active(true);
			Hive_Pod_Array[i].set_major_array_id(i);
			if (i + 1 == max_list_pointer) max_list_pointer++;
			num_current_hive_pods++;

			Pod_Grid_Map[new_grid_coords] = &Hive_Pod_Array[i];

			vec3 ship_start_pos = world_translation;
			vec3 ship_orbit_pos = world_translation;

			register_new_hive_ship(&Hive_Pod_Array[i], ship_start_pos, ship_orbit_pos);
			Manage_Obscurity(&Hive_Pod_Array[i]);
			update_hive_pod_model_matrices();
			break;
		}
	}
}

void Hive_Object::register_new_hive_ship(Hive_Pod_Object* hive_pod_pointer, vec3 translation_vector, vec3 orbit_center)
{
	hive_pod_pointer->hive_ship_pointer = hive_ship_array.add_ship_to_array(translation_vector, orbit_center);
}

vec3 Hive_Object::return_hive_color()
{
	return Hive_Color;
}

int Hive_Object::return_num_current_pods()
{
	return num_current_hive_pods;
}

Hive_Pod_Object * Hive_Object::return_hive_pod_at_array_num(int array_num)
{
	return &Hive_Pod_Array[array_num];
}

Hive_Pod_Object * Hive_Object::return_random_active_pod()
{
	int choice = rand() % max_list_pointer;

	if (Hive_Pod_Array[choice].is_active()) return &Hive_Pod_Array[choice];
}

mat4 Hive_Object::return_model_matrix()
{
	return TranslationMatrix*RotationMatrix*ScaleMatrix;
}

model_buffer_specs* Hive_Object::return_loaded_hive_specs()
{
	return &loaded_specs;
}

mat4 * Hive_Object::return_hive_pods_model_matrices()
{
	return hive_pod_model_matrices;
}

Hive_Ship_Array * Hive_Object::return_hive_ship_array()
{
	return &hive_ship_array;
}

Hive_Object * Hive_Object::return_hive_engagement_target()
{
	return hive_engagement_target;
}

vec3 Hive_Object::return_hive_translation_vector()
{
	return hive_translation_vector;
}

void Hive_Object::remove_hive_pod(Hive_Pod_Object* hive_pod)
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Hive_Pod_Array[i].return_major_array_id() == hive_pod->return_major_array_id() && Hive_Pod_Array[i].is_active() == true)
		{
			Hive_Pod_Array[i].set_active(false);
			if (i + 1 == max_list_pointer) max_list_pointer--;
			num_current_hive_pods--;
			break;
		}
	}
}

bool Hive_Object::set_hive_engagement_target(Hive_Object * _engagement_hive)
{
	hive_engagement_target = _engagement_hive;
	set_engaged(true);
	set_hive_ship_array_engagement_targets();
	return true;
}

void Hive_Object::set_engaged(bool engaged)
{
	hive_is_engaged = engaged;
}

void Hive_Object::set_hive_ship_array_engagement_targets()
{
	Hive_Ship_Array* hive_ship_array_target = hive_engagement_target->return_hive_ship_array();

	if (hive_ship_array_target != NULL)
	{
		hive_ship_array.set_ship_array_as_engagement_target(hive_ship_array_target);
		hive_ship_array.set_array_state(Hive_Ship_Array::SWARM_STATE_ENGAGED);
	}

}

void Hive_Object::update_hive_pod_model_matrices()
{
	int hive_pod_index = 0;

	for (int i = 0; i < max_list_pointer; i++)
	{
		Hive_Pod_Object* new_hive_pod_object = return_hive_pod_at_array_num(i);
		if (new_hive_pod_object != NULL && new_hive_pod_object->is_active())
		{
			hive_pod_model_matrices[hive_pod_index] = new_hive_pod_object->return_current_model_matrix();
			hive_pod_index++;
		}
	}
}

void Hive_Object::update_model_vertices()
{
	vertices.clear();
	uvs.clear();
	normals.clear();

	for (int i = 0; i < num_current_hive_pods; i++)
	{
		if (Hive_Pod_Array[i].is_active()) Hive_Pod_Array[i].return_octohedron_base()->add_triangle_array_components(vertices, uvs, normals);
	}

	indexed_vertices.clear();
	indexed_uvs.clear();
	indexed_normals.clear();
	indices.clear();

	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
}

void Hive_Object::update_ship_arrays()
{
	hive_ship_array.update_ships();

	if (hive_ship_array.return_num_pods_to_add_to_hive() > 0)
	{
		int num_pods_to_add = hive_ship_array.return_num_pods_to_add_to_hive();
		
		for (int i = 0; i < num_pods_to_add; i++)
		{
			extrude_new_octo();
		}

		hive_ship_array.decrement_num_pods_to_add_to_hive(num_pods_to_add);
	}
}

void Hive_Object::update_pod_array()
{
	for (int i = 0; i < max_list_pointer; i++)
	{
		if (Hive_Pod_Array[i].is_active() && Hive_Pod_Array[i].return_hive_pod_health() <= 0.0f)
		{
			remove_hive_pod(&Hive_Pod_Array[i]);
			update_hive_pod_model_matrices();
			num_current_hive_pods--;
		}
	}
}

void Hive_Object::update_translation_matrix()
{
	TranslationMatrix = translate(hive_translation_vector);
}

