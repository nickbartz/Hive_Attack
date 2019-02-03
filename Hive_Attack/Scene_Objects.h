#pragma once

#include<iostream>
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include<vector>
#include<map>
#include<iostream>

#include<Scene_Graph.h>
#include<Draw_Object.h>


using namespace std;
using namespace glm;

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

class Ship_Object
{
public:
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	float speed = (rand() % 5 +5) / 100.0;

	vec3 current_transform_vector = { 0.0,0.0,0.0 };
	vec3 current_rotation_vector = { 0.0,0.0,0.0 };
	vec3 current_direction_vector = { speed,0.0,0.0 };

	vec3 orbit_location = { 0.0,0.0,0.0 };
	float orbit_distance = 3;
	bool steady_orbit = false;
	bool clock_wise = false;
	bool direction = rand() % 2;

	float needed_rotation_angle = 0.0;

	mat4 ScaleMatrix = scale(vec3{ 0.4f,0.4f,0.4f });
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);

	buffer_specs loaded_specs;

	void ship_init(glm::vec3 start_location, vec3 _orbit_location);

	void update_rotation_matrix(glm::vec3 euler_rotation_vector);

	void increment_rotation_matrix(vec3 incremental_rotation_vector);

	void update_transform_matrix(glm::vec3 transform_vector);

	void increment_transform_matrix(glm::vec3 current_direction_vector);

	void load_buffer_return_specs();

	void rotate_directional_vector(float angle);

	void set_new_orbit(glm::vec3 _orbit_location);

	float calculate_distance_from_orbit_location();

	float calculate_distance_between_two_points(float x1, float z1, float x2, float z2);

	glm::vec3 return_closest_orbit_entry_point();

	void calculate_needed_rotation_increment_from_goal(glm::vec3 goal_position);

	void move();

	void check_nearby_targets();

private:
};

class Octohedron
{
public:
	Octohedron()
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

	map<string, vec3> octo_vertices;

	Hexagon hexagon_array[8];
	Square square_array[6];
	Ship_Object octo_ship;
	Grid_Coord octohedron_coordinates = { 0,0,0 };

	vec3 scale_vector;
	vec3 translation_vector;
	vec3 rotation_vector;

	bool all_squares_obscured = false;
	bool all_hexagons_obscured = false;
	int index_hash = rand() % 100000;
	float d = sqrt(5);
	
	void Init_Octo(glm::vec3 _translation_vector, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset);

	void init_squares(vec3 translation_vector);

	void init_hexagons(vec3 translation_vector);

	vec3 get_translation_vector();

	vec3 get_scaled_translation_vector();

	void add_triangle_array_components(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals);

	void check_for_total_obscurity();

	void update_translation(vec3 translation_vector);

private:
};

class Octohedron_Array
{
public:
	Octohedron_Array();

	Octohedron cell_array[100];
	int num_current_cells = 0;

	map<Grid_Coord, Octohedron*> array_map;

	// Vertices for the Octohedron Array
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	mat4 ScaleMatrix = glm::mat4(1.0);
	mat4 TranslationMatrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);

	// Vertices for the Ship Model
	std::vector<glm::vec3> ship_vertices;
	std::vector<glm::vec2> ship_uvs;
	std::vector<glm::vec3> ship_normals;
	std::vector<glm::vec3> ship_indexed_vertices;
	std::vector<glm::vec2> ship_indexed_uvs;
	std::vector<glm::vec3> ship_indexed_normals;
	std::vector<unsigned short> ship_indices;

	buffer_specs loaded_specs;
	buffer_specs loaded_ship_specs;

	void init_octohedron_array();

	void update_translation_matrix(vec3 translation);

	void register_new_octohedron(vec3 translation_vector, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset);

	void extrude_new_octo();

	void extrude_from_square(int array_index);

	void extrude_from_hexagon(int array_index);

	void Manage_Obscurity(Octohedron* octo);

	void update_model_vertices();

	void load_buffer_return_specs();

private:
};