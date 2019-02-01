// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include<vector>
#include<map>
#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include<2DText.h>
#include<controls.h>
#include<loadShader.h>
#include<texture.hpp>
#include<vboindexer.hpp>
#include<objloader.hpp>
#include<Draw_Object.h>

using namespace std;
using namespace glm;

struct Grid_Coord
{
	int grid_x = 0, grid_y = 0, grid_z = 0;

	void print()
	{
		cout << grid_x << ":" << grid_y << ":" << grid_z;
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

		normal = glm::cross((v1-v2),(v3-v2));
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
		down =  _down + translation_vector;
		right = _right + translation_vector;

		tri_array[0].init_tri_array(left, up, right);
		tri_array[1].init_tri_array(right, down, left);
	}

	void return_square_triangles(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
	{
		for (int i = 0; i < 2; i++)
		{
			tri_array[i].add_triangle_vertices(vertices);
			uvs.push_back({0.0,0.0});
			uvs.push_back({0.0,0.5});
			uvs.push_back({0.25,0.0});
			tri_array[i].add_triangle_normals(normals);
		}
	}
};

struct Ship_Object
{
	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	float speed = 0.05;

	vec3 current_transform_vector = { 0.0,0.0,0.0 };
	vec3 current_rotation_vector = { 0.0,0.0,0.0 };
	vec3 current_direction_vector = { speed,0.0,0.0 };

	vec3 orbit_location = { 0.0,0.0,0.0 };
	float orbit_distance = 1.5;
	bool steady_orbit = false;
	bool clock_wise = false;

	float needed_rotation_angle = 0.0;

	mat4 ScaleMatrix = glm::mat4(1.0);
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);

	buffer_specs loaded_specs;

	void ship_init(glm::vec3 start_location, vec3 _orbit_location)
	{
		loadOBJ_no_uvs("Basic_Ship.obj", vertices, uvs, normals);

		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

		update_scale_matrix({ 0.25,0.25,0.25 });
		update_transform_matrix(start_location);
		update_rotation_matrix({ 0.0,0.0,-1.55 });

		orbit_location = _orbit_location;
	}

	void update_rotation_matrix(glm::vec3 euler_rotation_vector)
	{
		current_rotation_vector = euler_rotation_vector;
		glm::quat MyQuaternion;
		glm::vec3 EulerAngles(current_rotation_vector);
		MyQuaternion = glm::quat(EulerAngles);
		RotationMatrix = glm::toMat4(MyQuaternion);
	}

	void increment_rotation_matrix(vec3 incremental_rotation_vector)
	{
		current_rotation_vector += incremental_rotation_vector;
		update_rotation_matrix(current_rotation_vector);
	}

	void update_scale_matrix(glm::vec3 scale_vector)
	{
		ScaleMatrix = glm::scale(scale_vector);
	}

	void update_transform_matrix(glm::vec3 transform_vector)
	{
		Transform_Matrix = glm::translate(transform_vector);
		current_transform_vector = transform_vector;
	}

	void increment_transform_matrix(glm::vec3 current_direction_vector)
	{
		glm::vec3 new_transform_vector = current_transform_vector + current_direction_vector;
		current_transform_vector = new_transform_vector;
		update_transform_matrix(current_transform_vector);
	}

	void load_buffer_return_specs()
	{
		loaded_specs = load_object_to_buffers(indexed_vertices, indexed_uvs, indexed_normals, indices);
	}

	void rotate_directional_vector(float angle)
	{
		float x_coord = ((current_direction_vector.x) * cos(angle)) - ((current_direction_vector.z)*sin(angle));
		float z_coord = ((current_direction_vector.z) * cos(angle)) + ((current_direction_vector.x)*sin(angle));

		current_direction_vector.x = x_coord;
		current_direction_vector.z = z_coord;
	}

	void set_new_orbit(glm::vec3 _orbit_location)
	{
		orbit_location = _orbit_location;
		steady_orbit = false;
	}

	float calculate_distance_from_orbit_location()
	{
		float px = current_transform_vector.x;
		float pz = current_transform_vector.z;
		float cx = orbit_location.x;
		float cz = orbit_location.z;

		float dx = abs(cx - px);
		float dz = abs(cz - pz);
		return sqrt(dx * dx + dz * dz);
	}

	float calculate_distance_between_two_points(float x1, float z1, float x2, float z2)
	{
		float dx = abs(x2 - x1);
		float dz = abs(z2 - z1);

		float dd = sqrt(dx * dx + dz * dz);

		return dd;
	}

	glm::vec3 return_closest_orbit_entry_point()
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

	void calculate_needed_rotation_increment_from_goal(glm::vec3 goal_position)
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
	
	void move()
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

			float grav_accel = speed;

			float body_vel_x = (sin(angle) * grav_accel);
			float body_vel_y = (cos(angle) * grav_accel);

			// This doesn't work, should be fixed to smooth entry
			current_direction_vector = { body_vel_x, 0.0, -body_vel_y };

			vec3 origin_direction = orbit_location - current_transform_vector;
			origin_direction *= distance / orbit_distance * 0.0001;

			increment_transform_matrix(origin_direction);	
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

};

struct Octohedron
{
	int index_hash = rand() % 100000;
	Hexagon hexagon_array[8];
	Square square_array[6];
	Grid_Coord octohedron_coordinates = { 0,0,0 };
	glm::vec3 translation_vector;
	bool all_squares_obscured = false;
	bool all_hexagons_obscured = false;

	vec3 scale_vector;

	Ship_Object octo_ship;


	float d = sqrt(5);

	glm::vec3 v1234 = { d, 0, 0 }; ;
	glm::vec3 v1243 = { 4 / d,	2 / d,	1 };
	glm::vec3 v1324 = { 4 / d, -3 / d,	0 };
	glm::vec3 v1342 = { 2 / d,	1 / d,	2 };
	glm::vec3 v1423 = { 2 / d, -4 / d, 1 };
	glm::vec3 v1432 = { 1 / d, -2 / d, 2 };

	glm::vec3 v2134 = { 4 / d, 2 / d, -1 };
	glm::vec3 v2143 = { 3 / d, 4 / d, 0 };
	glm::vec3 v2314 = { 2 / d,-4 / d,-1 };
	glm::vec3 v2341 = { -1 / d,2 / d,2 };
	glm::vec3 v2431 = { -2 / d,-1 / d,2 };
	glm::vec3 v2413 = { 0,-d,0 };

	glm::vec3 v3142 = -v2413;
	glm::vec3 v3124 = -v2431;
	glm::vec3 v3241 = -v2314;
	glm::vec3 v3214 = -v2341;
	glm::vec3 v3412 = -v2143;
	glm::vec3 v3421 = -v2134;

	glm::vec3 v4123 = -v1432;
	glm::vec3 v4132 = -v1423;
	glm::vec3 v4231 = -v1324;
	glm::vec3 v4213 = -v1342;
	glm::vec3 v4312 = -v1243;
	glm::vec3 v4321 = -v1234;

	void Init_Octo(glm::vec3 _translation_vector, glm::vec3 _scale_vector, Grid_Coord base_grid_coords, Grid_Coord face_grid_offset)
	{
		octohedron_coordinates.grid_x = base_grid_coords.grid_x + face_grid_offset.grid_x;
		octohedron_coordinates.grid_y = base_grid_coords.grid_y + face_grid_offset.grid_y;
		octohedron_coordinates.grid_z = base_grid_coords.grid_z + face_grid_offset.grid_z;

		translation_vector = _translation_vector;
		scale_vector = _scale_vector;

		Square new_square;
		new_square.init_square(v1324, v2314, v2413, v1423, translation_vector); // A+ (grid x + 2)
		new_square.set_tessalation_vector(v1324 - v3142);
		new_square.opposite_square = 1;
		new_square.face_grid_offset = { 2,0,0 };
		square_array[0] = (new_square);

		new_square.init_square(v3142, v3241, v4231, v4132, translation_vector); // A+ (grid x - 2)
		new_square.set_tessalation_vector(v3142 - v1324);
		new_square.opposite_square = 0;
		new_square.face_grid_offset = { -2,0,0 };
		square_array[1] = (new_square);

		new_square.init_square(v3124, v4123, v4213, v3214, translation_vector); // C ( grid z + 2)
		new_square.set_tessalation_vector(v3124 - v1342);
		new_square.opposite_square = 3;
		new_square.face_grid_offset = { 0,0,2 };
		square_array[2] = (new_square);

		new_square.init_square(v1432, v2431, v2341, v1342, translation_vector); // C ( grid z -2)
		new_square.set_tessalation_vector(v1432 - v3214);
		new_square.opposite_square = 2;
		new_square.face_grid_offset = { 0,0,-2 };
		square_array[3] = (new_square);

		new_square.init_square(v2143, v2134, v1234, v1243, translation_vector); // B ( y - 2)
		new_square.set_tessalation_vector(v2143 - v4321);
		new_square.opposite_square = 5;
		new_square.face_grid_offset = { 0,-2,0 };
		square_array[4] = (new_square);

		new_square.init_square(v3412, v4312, v4321, v3421, translation_vector); // B ( y + 2)
		new_square.set_tessalation_vector(v3412 - v1234);
		new_square.opposite_square = 4;
		new_square.face_grid_offset = { 0,2,0 };
		square_array[5] = (new_square);


		Hexagon new_hexagon;
		new_hexagon.init_hexagon(v2314, v3214, v4213, v4312, v3412, v2413, translation_vector); // C ( x + 1, z + 1, y + 1)
		new_hexagon.set_tessalation_vector(v2314 - v1243);
		new_hexagon.opposite_hexagon = 6;
		new_hexagon.face_grid_offset = { 1,1,1 };
		hexagon_array[0] = (new_hexagon);

		new_hexagon.init_hexagon(v4213, v4123, v4132, v4231, v4321, v4312, translation_vector); // D ( x - 1, z + 1, y + 1)
		new_hexagon.set_tessalation_vector(v4213 - v1324);
		new_hexagon.opposite_hexagon = 5;
		new_hexagon.face_grid_offset = { -1,1,1 };
		hexagon_array[1] = (new_hexagon);

		new_hexagon.init_hexagon(v2134, v3124, v3214, v2314, v1324, v1234, translation_vector); // B ( x + 1, z + 1, y-1)
		new_hexagon.set_tessalation_vector(v2134 - v3241);
		new_hexagon.opposite_hexagon = 7;
		new_hexagon.face_grid_offset = { 1,-1,1 };
		hexagon_array[2] = (new_hexagon);

		new_hexagon.init_hexagon(v4123, v3124, v2134, v2143, v3142, v4132, translation_vector); // A ( x- 1, z+1, y-1)
		new_hexagon.set_tessalation_vector(v4123 - v3412);
		new_hexagon.opposite_hexagon = 4;
		new_hexagon.face_grid_offset = { -1,-1,1 };
		hexagon_array[3] = (new_hexagon);

		new_hexagon.init_hexagon(v2413, v3412, v3421, v2431, v1432, v1423, translation_vector); // A ( x + 1, z - 1, y + 1)
		new_hexagon.set_tessalation_vector(v2413 - v3124);
		new_hexagon.opposite_hexagon = 3;
		new_hexagon.face_grid_offset = { 1,1,-1 };
		hexagon_array[4] = (new_hexagon);

		new_hexagon.init_hexagon(v1243, v1234, v1324, v1423, v1432, v1342, translation_vector); // D ( x + 1, z - 1, y - 1)
		new_hexagon.set_tessalation_vector(v1243 - v4132);
		new_hexagon.opposite_hexagon = 1;
		new_hexagon.face_grid_offset = { 1,-1,-1 };
		hexagon_array[5] = (new_hexagon);

		new_hexagon.init_hexagon(v3142, v2143, v1243, v1342, v2341, v3241, translation_vector); // C ( x - 1, z - 1, y - 1)
		new_hexagon.set_tessalation_vector(v3142 - v4213);
		new_hexagon.opposite_hexagon = 0;
		new_hexagon.face_grid_offset = { -1,-1,-1 };
		hexagon_array[6] = (new_hexagon);

		new_hexagon.init_hexagon(v3421, v4321, v4231, v3241, v2341, v2431, translation_vector); // B ( x - 1, z - 1, y+1)
		new_hexagon.set_tessalation_vector(v3421 - v2314);
		new_hexagon.opposite_hexagon = 2;
		new_hexagon.face_grid_offset = { -1,1,-1 };
		hexagon_array[7] = (new_hexagon);

		vec3 scaled_translation = { translation_vector.x*scale_vector.x, translation_vector.y*scale_vector.y, translation_vector.z*scale_vector.z };
		octo_ship.ship_init({ 0.0,0.0,0.0 }, scaled_translation);
		octo_ship.load_buffer_return_specs();

	}

	vec3 get_translation_vector()
	{
		return translation_vector;
	}

	vec3 get_scaled_translation_vector()
	{
		return { translation_vector.x*scale_vector.x, translation_vector.y*scale_vector.y, translation_vector.z*scale_vector.z };
	}

	void add_triangle_array_components(vector<glm::vec3> &vertices, vector<glm::vec2> &uvs, vector<glm::vec3> &normals)
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

	void check_for_total_obscurity()
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

};

struct Octohedron_Array
{
	Octohedron cell_array[100];
	int num_current_cells = 0;
	map<Grid_Coord, Octohedron*> array_map;

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	buffer_specs loaded_specs;

	vec3 scale_vector = { 0.25, 0.25, 0.25 };
	mat4 ScaleMatrix = glm::mat4(1.0);
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(0.0);

	void init_base_octo()
	{
		Octohedron base_octo;
		base_octo.Init_Octo({ 0.0f, 0.0f,0.0f }, scale_vector, { 0,0,0 }, { 0,0,0 }) ;
		cell_array[num_current_cells] = base_octo;
		array_map[base_octo.octohedron_coordinates] = &cell_array[num_current_cells];
		num_current_cells++;
	}

	void extrude_new_octo()
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

	void extrude_from_square(int array_index)
	{
		glm::vec3 translation_vector;
		int acceptable_faces[4] = { 0,1,2,3 };
		int face_index = rand() % 4;
		for (int i = 0; i < 4; i++)
		{
			if (cell_array[array_index].square_array[acceptable_faces[face_index]].return_obscured() == false)
			{
				translation_vector = cell_array[array_index].get_translation_vector() + cell_array[array_index].square_array[acceptable_faces[face_index]].tessallation_vector;
				Octohedron new_octo;
				new_octo.Init_Octo(translation_vector,scale_vector, cell_array[array_index].octohedron_coordinates, cell_array[array_index].square_array[acceptable_faces[face_index]].face_grid_offset);
				cell_array[num_current_cells] = new_octo;
				array_map[new_octo.octohedron_coordinates] = &cell_array[num_current_cells];		
				Manage_Obscurity(&cell_array[num_current_cells]);
				num_current_cells++;
				return;
			}

			face_index++;
			if (face_index > 3) face_index = 0;
		}

		cout << "could not find square to extrude from - this should be impossible" << endl;
	}

	void extrude_from_hexagon(int array_index)
	{
		// Find the first hexagon that isn't obscured
		int face_num = rand() % 8;
		glm::vec3 translation_vector;
		for (int i = 0; i < 8; i++)
		{
			if (cell_array[array_index].hexagon_array[face_num].return_obscured() == false)
			{
				translation_vector = cell_array[array_index].get_translation_vector() + cell_array[array_index].hexagon_array[face_num].tessallation_vector;
				Octohedron new_octo;
				new_octo.Init_Octo(translation_vector, scale_vector, cell_array[array_index].octohedron_coordinates, cell_array[array_index].hexagon_array[face_num].face_grid_offset);
				cell_array[num_current_cells] = new_octo;
				array_map[new_octo.octohedron_coordinates] = &cell_array[num_current_cells];
				Manage_Obscurity(&cell_array[num_current_cells]);
				num_current_cells++;
				return;
			}

			face_num++;
			if (face_num > 7) face_num = 0;
		}
	}

	void Manage_Obscurity(Octohedron* octo)
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

	void update_model_vertices()
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

	void update_scale_matrix(glm::vec3 scale_vector)
	{
		ScaleMatrix = glm::scale(scale_vector);
	}

	void update_transform_matrix(glm::vec3 transform_vector)
	{
		Transform_Matrix = glm::translate(transform_vector);
	}

	void load_buffer_return_specs()
	{
		loaded_specs = load_object_to_buffers(indexed_vertices, indexed_uvs, indexed_normals, indices);
	}

	void update_rotation_matrix(glm::vec3 euler_rotation_vector)
	{
		glm::quat MyQuaternion;
		glm::vec3 EulerAngles(euler_rotation_vector);
		MyQuaternion = glm::quat(EulerAngles);
		RotationMatrix = glm::toMat4(MyQuaternion);
	}
};

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initalize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	GLFWwindow* window;

	window = glfwCreateWindow(1024, 768, "Open_GL_Test", NULL, NULL);
	glfwSetWindowPos(window, 400, 400);

	if (window == NULL)
	{
		fprintf(stderr, "Failed to open window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed ot initalize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardTransparentShading.fragmentshader");

	Octohedron_Array new_array;
	new_array.init_base_octo();
	new_array.update_scale_matrix(new_array.scale_vector);
	new_array.update_rotation_matrix({ 0.0,0.0,1.1 });
	new_array.update_transform_matrix({ 0.0,0.0,0.0 });
	for (int i = 0; i < 10; i++) new_array.extrude_new_octo();
	new_array.update_model_vertices();
	new_array.load_buffer_return_specs();

	Octohedron_Array new_array_2;
	new_array_2.init_base_octo();
	new_array_2.update_scale_matrix(new_array_2.scale_vector);
	new_array_2.update_rotation_matrix({ 0.0,0.0,1.1 });
	new_array_2.update_transform_matrix({ 20.0,0.0,0.0 });
	for (int i = 0; i < 5; i++) new_array_2.extrude_new_octo();
	new_array_2.update_model_vertices();
	new_array_2.load_buffer_return_specs();


	// Get a handle for our "LightPosition" uniform

	glm::vec3 lightPos = glm::vec3(10, 10, 10);

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	float addition = 0.1;

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	do {

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
											 // printf and reset
			printf("%f ms/frame\n", double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs(window);
		assign_uniform_pointers(programID);
		
		Draw_Object(window, programID, new_array.loaded_specs, lightPos, new_array.ScaleMatrix, new_array.Transform_Matrix,new_array.RotationMatrix);
		Draw_Object(window, programID, new_array_2.loaded_specs, lightPos, new_array_2.ScaleMatrix, new_array_2.Transform_Matrix, new_array_2.RotationMatrix);

		for (int i = 0; i < new_array.num_current_cells; i++)
		{
			Draw_Object(window, programID, new_array.cell_array[i].octo_ship.loaded_specs, lightPos, new_array.cell_array[i].octo_ship.ScaleMatrix, new_array.cell_array[i].octo_ship.Transform_Matrix, new_array.cell_array[i].octo_ship.RotationMatrix);
			new_array.cell_array[i].octo_ship.move();
		}

		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			for (int i = 0; i < new_array.num_current_cells; i++)
			{
				new_array.cell_array[i].octo_ship.set_new_orbit({20.0,0.0,0.0});
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		{
			for (int i = 0; i < new_array.num_current_cells; i++)
			{
				new_array.cell_array[i].octo_ship.set_new_orbit(new_array.cell_array[i].get_scaled_translation_vector());
			}
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	Cleanup_Object(&new_array.loaded_specs);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}