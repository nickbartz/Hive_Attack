#include<Gameplay_Manager.h>
#include<Object_Hive.h>
#include<Object_Hive_Swarm.h>
#include<Object_Hive_Ship.h>
#include<Object_Hive_Pod.h>
#include<vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Render_Manager.h>
#include<Service_Locator.h>
#include<Memory_Manager.h>

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

Hive_Object::Hive_Object()
{

}

void Hive_Object::Init_Hive_Object(Service_Locator* _service_locator, vec3 initial_location, vec3 _Hive_Color, float hive_damage, model_buffer_specs* _hive_pod_model, model_buffer_specs* hive_ship_model)
{
	set_active(true);

	service_locator = _service_locator;
	glm::quat MyQuaternion;
	glm::vec3 EulerAngles({ 0.0f,0.0f,0.0f });
	MyQuaternion = glm::quat(EulerAngles);
	RotationMatrix = glm::toMat4(MyQuaternion);

	Hive_Color = _Hive_Color;
	Hive_Damage = hive_damage;
	Hive_Ship_Model = hive_ship_model;
	hive_translation_vector = initial_location;

	Create_New_Swarm(Hive_Color, hive_damage, hive_ship_model);

	register_new_hive_pod({ 0.0,0.0,0.0 }, hive_translation_vector, { 0,0,0 }, { 0,0,0 }, Hive_Color);

	hive_pod_model = _hive_pod_model;

	uniq_id = rand() % 10000;
}

Hive_Ship_Array* Hive_Object::Create_New_Swarm(vec3 Hive_Color, float hive_damage, model_buffer_specs* hive_ship_model)
{
	// ADD THIS TO THE MEMORY MANAGER
	Hive_Ship_Array* new_hive_ship_array = service_locator->return_memory_manager()->new_hive_ship_array();
	new_hive_ship_array->init_hive_ship_array(service_locator, Hive_Color, hive_damage, hive_ship_model);
	hive_swarm_array.push_back(new_hive_ship_array);

	return new_hive_ship_array;
}

void Hive_Object::coalesce_idle_swarms()
{
	Hive_Ship_Array* base_idle_array = NULL;

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE)
		{
			if (base_idle_array == NULL)
			{
				base_idle_array = hive_swarm_array[i];
			}
			else
			{
				for (int p = 0; p < hive_swarm_array[i]->return_num_ships_in_swarm(); p++)
				{
					Ship_Object* new_ship = hive_swarm_array[i]->return_ship_by_array_num(p);

					if (new_ship != NULL)
					{
						base_idle_array->add_existing_ship_to_array(new_ship);
						hive_swarm_array[i]->remove_ship_from_array(new_ship);
					}
				}

				hive_swarm_array.erase(hive_swarm_array.begin() + i);
				break;
			}
		}
	}
}

void Hive_Object::destroy()
{
	set_active(false);

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		hive_swarm_array[i]->destroy();
	}
}

void Hive_Object::extrude_new_octo(vec3 octo_color)
{
	int array_index = 0;
	bool square_found = false;
	int face_num = 0;

	bool extrusion_found = false;

	// Try to find a cell where not all squares are obscured
	array_index = rand() % Hive_Pod_Array.size();

	for (int i = 0; i < num_current_hive_pods; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index]->return_octohedron_base();

		if (base_octohedron->check_for_total_obscurity() == false)
		{
			/*if (rand() % 2 == 0) extrude_from_square(array_index, octo_color);
			else 
			*/
			extrude_from_hexagon(array_index, octo_color);
			return;
		}


		array_index++;
		if (array_index > num_current_hive_pods - 1) array_index = 0;
	}
}

void Hive_Object::extrude_from_square(int array_index, vec3 octo_color)
{
	int acceptable_faces[4] = { 0,1,2,3 };
	int face_index = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index]->return_octohedron_base();

		if (base_octohedron->square_array[acceptable_faces[face_index]].return_obscured() == false)
		{
			vec3 local_position = Hive_Pod_Array[array_index]->return_local_translation_vector() + base_octohedron->square_array[acceptable_faces[face_index]].tessallation_vector;
			vec3 world_position = local_position + hive_translation_vector;

			register_new_hive_pod(local_position, world_position, Hive_Pod_Array[array_index]->octohedron_coordinates, base_octohedron->square_array[acceptable_faces[face_index]].face_grid_offset, octo_color);

			return;
		}

		face_index++;
		if (face_index > 3) face_index = 0;
	}

	cout << "could not find square to extrude from - this should be impossible" << endl;
}

void Hive_Object::extrude_from_hexagon(int array_index, vec3 octo_color)
{
	// Find the first hexagon that isn't obscured
	int face_num = rand() % 8;
	for (int i = 0; i < 8; i++)
	{
		Octohedron_Model* base_octohedron = Hive_Pod_Array[array_index]->return_octohedron_base();
		if (base_octohedron->hexagon_array[face_num].return_obscured() == false)
		{
			vec3 local_position = Hive_Pod_Array[array_index]->return_local_translation_vector() + base_octohedron->hexagon_array[face_num].tessallation_vector;
			vec3 world_position = local_position + hive_translation_vector;

			register_new_hive_pod(local_position, world_position, Hive_Pod_Array[array_index]->octohedron_coordinates, base_octohedron->hexagon_array[face_num].face_grid_offset, octo_color);

			return;
		}

		face_num++;
		if (face_num > 7) face_num = 0;
	}
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

void Hive_Object::register_new_hive_pod(vec3 local_translation,vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset, vec3 hive_pod_color)
{
	Hive_Pod_Object* new_octo = service_locator->return_memory_manager()->new_hive_pod_object();

	new_octo->Init_Hive_Pod(service_locator, local_translation, world_translation, base_grid_coords, face_grid_offset, hive_pod_color);
	Grid_Coord new_grid_coords = new_octo->octohedron_coordinates;
	new_octo->set_active(true);
	num_current_hive_pods++;

	Pod_Grid_Map[new_grid_coords] = new_octo;
	vec3 ship_start_pos = world_translation;
	vec3 ship_orbit_pos = world_translation;
	register_new_hive_ship(new_octo, ship_start_pos, ship_orbit_pos);

	Manage_Obscurity(new_octo);
	
	Hive_Pod_Array.push_back(new_octo);

	update_all_hive_pod_model_matrices();
	update_hive_pod_color_matrix();
}

void Hive_Object::register_new_hive_ship(Hive_Pod_Object* hive_pod_pointer, vec3 translation_vector, vec3 orbit_center)
{
	if (hive_swarm_array.size() <= 0)
	{
		Create_New_Swarm(Hive_Color, Hive_Damage, Hive_Ship_Model);
	}

	Ship_Object* new_ship = hive_swarm_array.back()->add_new_ship_to_array(translation_vector, orbit_center);
	hive_pod_pointer->register_hive_ship(new_ship);

}

float Hive_Object::return_aware_radius()
{
	return aware_radius;
}

vec3 Hive_Object::return_hive_color()
{
	return Hive_Color;
}

int Hive_Object::return_num_current_pods()
{
	return num_current_hive_pods;
}

int Hive_Object::return_num_current_pods_not_currently_being_plundered()
{
	int count = 0;

	for (int i = 0; i < Hive_Pod_Array.size(); i++)
	{
		if (Hive_Pod_Array[i]->return_pod_is_being_plundered() == false) count++;
	}

	return count;
}

Hive_Ship_Array * Hive_Object::return_nearest_hive_array(vec3 location)
{
	Hive_Ship_Array* nearest_array = NULL;
	float distance = -1.0;

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		float swarm_distance = glm::distance(location, hive_swarm_array[i]->return_current_location());

		if (swarm_distance < distance || distance == -1.0)
		{
			nearest_array = hive_swarm_array[i];
			distance = swarm_distance;
		}
	}

	return nearest_array;
}

Hive_Pod_Object * Hive_Object::return_random_active_pod()
{
	int choice = rand() % num_current_hive_pods;

	if (Hive_Pod_Array[choice]->is_active()) return Hive_Pod_Array[choice];
	else return NULL;
}

mat4 Hive_Object::return_model_matrix()
{
	return TranslationMatrix*RotationMatrix*ScaleMatrix;
}

model_buffer_specs* Hive_Object::return_loaded_hive_pod_model()
{
	return hive_pod_model;
}

mat4 * Hive_Object::return_hive_pods_model_matrices()
{
	return hive_pod_model_matrices;
}

vec3 * Hive_Object::return_hive_pods_color_matrices()
{
	return hive_pod_color_matrices;
}

int Hive_Object::return_uniq_id()
{
	return uniq_id;
}

bool Hive_Object::is_active()
{
	return active;
}

int Hive_Object::return_num_idle_swarms()
{
	int count = 0;

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE && hive_swarm_array[i]->return_num_ships_in_swarm() > 0)
		{
			count++;
		}
	};

	return count;
}

vec3 Hive_Object::return_hive_translation_vector()
{
	return hive_translation_vector;
}

int Hive_Object::return_num_ship_arrays()
{
	return hive_swarm_array.size();
}

int Hive_Object::return_total_ships_in_swarm()
{
	int count = 0;

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		count += hive_swarm_array[i]->return_num_ships_in_swarm();
	}

	return count;
}

Hive_Ship_Array* Hive_Object::return_ship_array_by_num(int num)
{
	return hive_swarm_array[num];
}

void Hive_Object::remove_hive_pod(Hive_Pod_Object* hive_pod)
{
	service_locator->return_memory_manager()->Deallocate_Hive_Pod_Object(hive_pod);

	for (int i = 0; i < num_current_hive_pods; i++)
	{
		if (Hive_Pod_Array[i] == hive_pod)
		{
			Hive_Pod_Array.erase(Hive_Pod_Array.begin() + i);
			num_current_hive_pods--;
			break;
		}
	}
}

void Hive_Object::set_active(bool _active)
{
	active = _active;
}

Hive_Ship_Array * Hive_Object::Split_New_Swarm_From_Idle_Swarm(int num_ships)
{
	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE)
		{
			if (hive_swarm_array[i]->return_num_ships_in_swarm() <= num_ships)
			{
				return hive_swarm_array[i];
			}
			else
			{
				Hive_Ship_Array* new_swarm = Create_New_Swarm(Hive_Color, Hive_Damage, Hive_Ship_Model);

				// THIS IS AWKWARD CONSIDER CHANGING 
				for (int p = 0; p < num_ships; p++)
				{
					Ship_Object* transferred_ship = hive_swarm_array[i]->return_ship_by_array_num(0);

					new_swarm->add_existing_ship_to_array(transferred_ship);
					hive_swarm_array[i]->remove_ship_from_array(transferred_ship);
				}
				return new_swarm;
			}

			break;
		}
	}

	return NULL;
}


void Hive_Object::target_respond_to_attacking_swarm(Hive_Ship_Array * attacking_swarm)
{
	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_num_ships_in_swarm() > 0 && hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE)
		{
			if (attacking_swarm != NULL && attacking_swarm->is_active())
			{
				hive_swarm_array[i]->change_swarm_target(attacking_swarm, NULL);
			}
		}
	}
}

void Hive_Object::target_add_plunderable_hive(Hive_Object * hive_object)
{
	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_num_ships_in_swarm() > 0 && hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE)
		{
			if (hive_object != NULL && hive_object->is_active())
			{
				hive_swarm_array[i]->change_swarm_target(NULL, hive_object);
			}
		}
	}
}

void Hive_Object::target_add_conquerable_swarm(Hive_Ship_Array * swarm)
{
	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_num_ships_in_swarm() > 0 && hive_swarm_array[i]->return_current_state() == Hive_Ship_Array::SWARM_STATE_IDLE)
		{
			if (swarm != NULL && swarm->is_active())
			{
				hive_swarm_array[i]->change_swarm_target(swarm, NULL);
			}
		}
	}
}

void Hive_Object::update()
{
	update_hive_swarms();

	update_pod_array();
}

void Hive_Object::update_hive_swarms()
{
	if (hive_swarm_array.size() > 1) coalesce_idle_swarms();

	for (int i = 0; i < hive_swarm_array.size(); i++)
	{
		if (hive_swarm_array[i]->return_num_ships_in_swarm() <= 0)
		{
			hive_swarm_array.erase(hive_swarm_array.begin() + i);
			break;
		}
		else
		{
			hive_swarm_array[i]->update();

			if (hive_swarm_array[i]->return_num_pods_to_add_to_hive() > 0)
			{
				vector<vec3>* pods_to_add_to_hive = hive_swarm_array[i]->return_pods_to_add_to_hive();
				for (int i = 0; i < pods_to_add_to_hive->size(); i++)
				{
					extrude_new_octo(pods_to_add_to_hive->back());
					pods_to_add_to_hive->pop_back();
				}
			}
		}
	}
}

void Hive_Object::update_all_hive_pod_model_matrices()
{
	int hive_pod_index = 0;

	for (int i = 0; i < Hive_Pod_Array.size(); i++)
	{
		Hive_Pod_Object* new_hive_pod_object = Hive_Pod_Array[i];
		if (new_hive_pod_object != NULL && new_hive_pod_object->is_active())
		{
			hive_pod_model_matrices[hive_pod_index] = new_hive_pod_object->return_current_model_matrix();
			new_hive_pod_object->set_current_matrix_pointer(hive_pod_index);
			hive_pod_index++;
		}
	}
}

void Hive_Object::update_single_hive_pod_model_matrix(Hive_Pod_Object* hive_pod)
{
	hive_pod_model_matrices[hive_pod->return_current_matrix_pointer()] = hive_pod->return_current_model_matrix();
}

void Hive_Object::update_hive_pod_color_matrix()
{
	int hive_pod_index = 0;

	for (int i = 0; i < Hive_Pod_Array.size(); i++)
	{
		Hive_Pod_Object* new_hive_pod_object = Hive_Pod_Array[i];
		if (new_hive_pod_object != NULL && new_hive_pod_object->is_active())
		{
			hive_pod_color_matrices[hive_pod_index] = new_hive_pod_object->return_pod_color();
			hive_pod_index++;
		}
	}
}

void Hive_Object::update_pod_array()
{
	for (int i = 0; i < Hive_Pod_Array.size(); i++)
	{
		Hive_Pod_Array[i]->update();

		if (Hive_Pod_Array[i]->is_able_to_create_ship())
		{
			register_new_hive_ship(Hive_Pod_Array[i], Hive_Pod_Array[i]->return_world_translation_vector(), Hive_Pod_Array[i]->return_world_translation_vector());
		}
	}
}

void Hive_Object::update_translation_matrix()
{
	TranslationMatrix = translate(hive_translation_vector);
}

