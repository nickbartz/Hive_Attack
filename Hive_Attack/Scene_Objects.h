#pragma once

#include<iostream>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include<vector>
#include<map>
#include<iostream>

#include<Draw_Object.h>
#include<Ship_Array.h>

using namespace std;
using namespace glm;

class Hive_Object;
class Hive_Pod_Object;

const int MAX_NUM_HIVE_PODS_PER_HIVE = 1000;

struct Grid_Coord
{
	int grid_x = 0, grid_y = 0, grid_z = 0;

	void print()
	{
		std::cout << grid_x << ":" << grid_y << ":" << grid_z;
	}

};

inline bool operator<(const Grid_Coord& p1, const Grid_Coord& p2) {
	if (p1.grid_x != p2.grid_x) {
		return p1.grid_x < p2.grid_x;
	}
	else if (p1.grid_y != p2.grid_y) {
		return p1.grid_y < p2.grid_y;
	}
	else
	{
		return p1.grid_z < p2.grid_z;
	}
}

struct Triangle
{
	glm::vec3 v_1;
	glm::vec3 v_2;
	glm::vec3 v_3;
	glm::vec3 normal;

	void init_tri_array(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
	{
		v_1 = v1;
		v_2 = v2;
		v_3 = v3;

		normal = glm::cross((v1 - v2), (v3 - v2));
	}

	void add_triangle_vertices(vector<glm::vec3> &vertex_array)
	{

		vertex_array.push_back(v_1);
		vertex_array.push_back(v_2);
		vertex_array.push_back(v_3);
	}

	void add_triangle_normals(vector<glm::vec3> &normal_array)
	{
		normal_array.push_back(normal);
		normal_array.push_back(normal);
		normal_array.push_back(normal);
	}

	void add_triangle_UVs(vector<glm::vec2> &uv_array)
	{
		uv_array.push_back({ 0.0f, 0.0f });
		uv_array.push_back({ 0.0f, 0.0f });
		uv_array.push_back({ 0.0f, 0.0f });
	}

};

struct Hexagon
{
	Triangle tri_array[4];
	glm::vec3 v_1;
	glm::vec3 v_2;
	glm::vec3 v_3;
	glm::vec3 v_4;
	glm::vec3 v_5;
	glm::vec3 v_6;
	glm::vec3 tessallation_vector;
	Grid_Coord face_grid_offset;
	int opposite_hexagon = 0;

	bool obscured = false;

	void set_obscured(bool _obscured)
	{
		obscured = _obscured;
	}

	bool return_obscured()
	{
		return obscured;
	}

	void init_hexagon(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 v5, glm::vec3 v6, glm::vec3 translation_vector = { 0.0f,0.0f,0.0f })
	{
		v_1 = v1 + translation_vector;
		v_2 = v2 + translation_vector;
		v_3 = v3 + translation_vector;
		v_4 = v4 + translation_vector;
		v_5 = v5 + translation_vector;
		v_6 = v6 + translation_vector;

		tri_array[0].init_tri_array(v_1, v_2, v_3);
		tri_array[1].init_tri_array(v_6, v_1, v_3);
		tri_array[2].init_tri_array(v_6, v_3, v_5);
		tri_array[3].init_tri_array(v_5, v_3, v_4);
	}

	void set_tessalation_vector(glm::vec3 _tesslation_vector)
	{
		tessallation_vector = _tesslation_vector;
	}

	void return_hexagon_triangles(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
	{
		for (int i = 0; i < 4; i++)
		{
			tri_array[i].add_triangle_vertices(vertices);
			tri_array[i].add_triangle_UVs(uvs);
			tri_array[i].add_triangle_normals(normals);
		}
	}
};

struct Square
{
	Triangle tri_array[2];
	glm::vec3 left;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 down;
	glm::vec3 tessallation_vector;
	Grid_Coord face_grid_offset;
	int opposite_square = 0;
	bool obscured = false;

	void set_obscured(bool _obscured)
	{
		obscured = _obscured;
	}

	bool return_obscured()
	{
		return obscured;
	}

	void set_tessalation_vector(glm::vec3 _tesslation_vector)
	{
		tessallation_vector = _tesslation_vector;
	}

	void init_square(glm::vec3 _left, glm::vec3 _up, glm::vec3 _right, glm::vec3 _down, glm::vec3 translation_vector = { 0.0f,0.0f, 0.0f })
	{
		left = _left + translation_vector;
		up = _up + translation_vector;
		down = _down + translation_vector;
		right = _right + translation_vector;

		tri_array[0].init_tri_array(left, up, right);
		tri_array[1].init_tri_array(right, down, left);
	}

	void return_square_triangles(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
	{
		for (int i = 0; i < 2; i++)
		{
			tri_array[i].add_triangle_vertices(vertices);
			uvs.push_back({ 0.0,0.0 });
			uvs.push_back({ 0.0,0.5 });
			uvs.push_back({ 0.25,0.0 });
			tri_array[i].add_triangle_normals(normals);
		}
	}
};

struct Octohedron_Model
{
	map<string, vec3> octo_vertices;

	float d = sqrt(5);

	void init_base_octohedron()
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
	}


};

class Hive_Pod_Object
{
public:
	Hive_Pod_Object()
	{
		base_octohedron = new Octohedron_Model;
		base_octohedron->init_base_octohedron();

		//octo_vertices["v1234"] = { d, 0, 0 }; ;
		//octo_vertices["v1243"] = { 4 / d,	2 / d,	1 };
		//octo_vertices["v1324"] = { 4 / d, -3 / d,	0 };
		//octo_vertices["v1342"] = { 2 / d,	1 / d,	2 };
		//octo_vertices["v1423"] = { 2 / d, -4 / d, 1 };
		//octo_vertices["v1432"] = { 1 / d, -2 / d, 2 };

		//octo_vertices["v2134"] = { 4 / d, 2 / d, -1 };
		//octo_vertices["v2143"] = { 3 / d, 4 / d, 0 };
		//octo_vertices["v2314"] = { 2 / d,-4 / d,-1 };
		//octo_vertices["v2341"] = { -1 / d,2 / d,2 };
		//octo_vertices["v2431"] = { -2 / d,-1 / d,2 };
		//octo_vertices["v2413"] = { 0,-d,0 };

		//octo_vertices["v3142"] = -octo_vertices["v2413"];
		//octo_vertices["v3124"] = -octo_vertices["v2431"];
		//octo_vertices["v3241"] = -octo_vertices["v2314"];
		//octo_vertices["v3214"] = -octo_vertices["v2341"];
		//octo_vertices["v3412"] = -octo_vertices["v2143"];
		//octo_vertices["v3421"] = -octo_vertices["v2134"];

		//octo_vertices["v4123"] = -octo_vertices["v1432"];
		//octo_vertices["v4132"] = -octo_vertices["v1423"];
		//octo_vertices["v4231"] = -octo_vertices["v1324"];
		//octo_vertices["v4213"] = -octo_vertices["v1342"];
		//octo_vertices["v4312"] = -octo_vertices["v1243"];
		//octo_vertices["v4321"] = -octo_vertices["v1234"];

		//map<string, vec3>::iterator it;

		//glm::quat MyQuaternion;
		//glm::vec3 EulerAngles({ 0.0f,0.0f,1.1f });
		//MyQuaternion = glm::quat(EulerAngles);

		//for (it = octo_vertices.begin(); it != octo_vertices.end(); it++)
		//{
		//	it->second *= 0.5;
		//	it->second = rotate(MyQuaternion, it->second);
		//}
	}

	map<string, vec3> octo_vertices;

	Hexagon hexagon_array[8];
	Square square_array[6];
	Ship_Object* hive_ship_pointer = NULL;
	Grid_Coord octohedron_coordinates;
	Octohedron_Model* base_octohedron;

	vec3 scale_vector;
	vec3 local_translation_vector;
	vec3 world_translation_vector;
	vec3 rotation_vector;

	bool all_squares_obscured = false;
	bool all_hexagons_obscured = false;
	int index_hash = rand() % 100000;
	float d = sqrt(5);

	
	void Init_Hive_Pod(glm::vec3 local_translation, vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset);

	void init_squares(vec3 translation_vector);

	void init_hexagons(vec3 translation_vector);

	vec3 return_local_translation_vector();

	vec3 return_world_translation_vector();

	vec3 return_scaled_translation_vector();

	void add_triangle_array_components(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);

	void check_for_total_obscurity();

	void update_translation(vec3 local_translation,vec3 world_translation);

	void register_hive_ship(Ship_Object* hive_ship);

	void take_damage(float damage);

	float return_hive_pod_health();

	bool is_active();

	void set_active(bool active);

	void set_major_array_id(int array_id);

	int return_major_array_id();

	mat4 return_current_model_matrix();

private:
	bool pod_is_active = false;
	int major_array_id = 0;
	float hive_pod_health = 100.0f;

	mat4 ScaleMatrix = glm::mat4(1.0);;
	mat4 Transform_Matrix = translate(vec3{0.0f,0.0f,0.0f});
	mat4 RotationMatrix = glm::mat4(1.0);
};

class Hive_Object
{
public:
	Hive_Object();

	Hive_Ship_Array hive_ship_array;
	
	Hive_Pod_Object Hive_Pod_Array[100];
	int num_current_hive_pods = 0;
	int max_list_pointer = 1;

	Hive_Object* hive_engagement_target = NULL;
	bool hive_is_engaged = false;

	map<Grid_Coord, Hive_Pod_Object*> Pod_Grid_Map;

	// Vertices for the Octohedron Array
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	vec3 hive_translation_vector;

	mat4 ScaleMatrix = glm::mat4(1.0);
	mat4 TranslationMatrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);

	vec3 Hive_Color = { 1.0,1.0,1.0 };

	model_buffer_specs loaded_specs;


	bool check_engagement_target_fleet_destroyed();

	void Init_Hive_Object(vec3 initial_location, vec3 Hive_Color, float ship_damage);

	void update_translation_matrix();


	void extrude_new_octo();

	void extrude_from_square(int array_index);

	void extrude_from_hexagon(int array_index);

	void Manage_Obscurity(Hive_Pod_Object* octo);

	void update_model_vertices();

	void update_ship_arrays();

	void update_pod_array();

	void load_buffer_return_specs();

	vec3 return_hive_color();

	int return_num_current_pods();

	Hive_Pod_Object* return_hive_pod_at_array_num(int array_num);

	Hive_Pod_Object* return_random_active_pod();

	model_buffer_specs* return_loaded_hive_specs();

	void register_new_hive_pod(vec3 local_translation, vec3 world_translation, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset);

	void remove_hive_pod(Hive_Pod_Object* hive_pod);

	void register_new_hive_ship(Hive_Pod_Object* hive_pod_pointer, vec3 translation_vector, vec3 orbit_center);

	mat4 return_model_matrix();

	void update_hive_pod_model_matrices();

	mat4* return_hive_pods_model_matrices();

	Hive_Object* return_hive_engagement_target();

	Hive_Ship_Array* return_hive_ship_array();

	bool set_hive_engagement_target(Hive_Object* engagement_hive);

	bool is_engaged();

	bool is_fleet_destroyed();

	void set_engaged(bool engaged);



private:
	mat4 hive_pod_model_matrices[MAX_NUM_HIVE_PODS_PER_HIVE];
	void set_hive_ship_array_engagement_targets();

};