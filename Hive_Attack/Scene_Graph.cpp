#include<Scene_Graph.h>
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
#include<Scene_Objects.h>

using namespace std;
using namespace glm;

Octohedron_Array new_array;
Octohedron_Array new_array_2;

struct ship_node
{
	Ship_Object* ship;
	ship_node* next;
	bool active = false; // for the non-linked list version
	int faction = 0; 
};

struct Ship_Linked_List
{
	ship_node *head, *tail;
	Ship_Linked_List()
	{
		head = NULL;
		tail = NULL;
	}

	void insert_end(Ship_Object* new_ship)
	{
		ship_node *temp = new ship_node;
		temp->ship = new_ship;
		temp->next = NULL;
		if (head == NULL)
		{
			head = temp;
			tail = temp;
			temp = NULL;
		}
		else
		{
			tail->next = temp;
			tail = temp;
		}
	}

	void display()
	{
		ship_node *temp = new ship_node;
		temp = head;
		while (temp != NULL)
		{
			cout << temp->ship << endl;
			temp = temp->next;
		}
	}

	void insert_start(Ship_Object* new_ship)
	{
		ship_node *temp = new ship_node;
		temp->ship = new_ship;
		temp->next = head;
		head = temp;
	}

	void insert_position(int pos, Ship_Object* new_ship)
	{
		ship_node* pre = new ship_node;
		ship_node* cur = new ship_node;
		ship_node* temp = new ship_node;
		cur = head;
		for (int i = 1; i < pos; i++)
		{
			pre = cur;
			cur = cur->next;
		}
		temp->ship = new_ship;
		pre->next = temp;
		temp->next = cur;
	}

	void delete_first()
	{
		ship_node *temp = new ship_node;
		temp = head;
		head = head->next;
		delete temp;
	}

	void delete_last()
	{
		ship_node* current = new ship_node;
		ship_node* previous = new ship_node;
		current = head;
		while (current->next != NULL)
		{
			previous = current;
			current = current->next;
		}
		tail = previous;
		previous->next = NULL;
		delete current;
	}

	void delete_position(int pos)
	{
		ship_node* current = new ship_node;
		ship_node* previous = new ship_node;
		current = head;
		for (int i = 1; i < pos; i++)
		{
			previous = current;
			current = current->next;
		}
		previous->next = current->next;
	}
};

struct Ship_Contiguous_List
{
	ship_node* ship_node_array[MAX_NUM_SHIPS_IN_MANIFEST];
	int num_ships_in_manifest = 0;
	int max_list_pointer = 1;

	void add_ship_to_manifest(Ship_Object* ship)
	{
		for (int i = 0; i < max_list_pointer; i++)
		{
			if (ship_node_array[i]->active == false)
			{
				ship_node_array[i]->ship = ship;
				ship_node_array[i]->active = true;
				if (i + 1 == max_list_pointer) max_list_pointer++;
				num_ships_in_manifest++;
				break;
			}
		}
	}

	void remove_ship_from_manifest(Ship_Object* ship)
	{
		for (int i = 0; i < max_list_pointer; i++)
		{
			if (ship_node_array[i]->ship == ship && ship_node_array[i]->active == true)
			{
				ship_node_array[i]->active = false;
				if (i + 1 == max_list_pointer) max_list_pointer--;
				num_ships_in_manifest--;
				break;
			}
		}
	}

};

Ship_Contiguous_List ship_manifest;

void init_scene_graph()
{
	new_array.init_octohedron_array();
	for (int i = 0; i < 10; i++) new_array.extrude_new_octo();
	new_array.update_model_vertices();
	new_array.load_buffer_return_specs();

	new_array_2.init_octohedron_array();
	for (int i = 0; i < 10; i++) new_array_2.extrude_new_octo();
	new_array_2.update_translation_matrix({ 20.0,0.0,0.0 });
	new_array_2.update_model_vertices();
	new_array_2.load_buffer_return_specs();
}

void cleanup_scene_graph()
{
	Cleanup_Object(&new_array.loaded_specs);
}

void update_scene_graph()
{
	for (int i = 0; i < new_array.num_current_cells; i++)
	{
		new_array.cell_array[i].octo_ship.move();
	}

	for (int i = 0; i < new_array_2.num_current_cells; i++)
	{
		new_array_2.cell_array[i].octo_ship.move();
	}
}

void draw_scene_graph(GLFWwindow* window, GLuint shader_program, glm::vec3 lightPos)
{
	Draw_Object(window, shader_program, new_array.loaded_specs, lightPos, new_array.ScaleMatrix, new_array.TranslationMatrix, new_array.RotationMatrix, {0.75,0.75,0.0});

	for (int i = 0; i < new_array.num_current_cells; i++)
	{
		Draw_Object(window, shader_program, new_array.loaded_ship_specs, lightPos, new_array.cell_array[i].octo_ship.ScaleMatrix, new_array.cell_array[i].octo_ship.Transform_Matrix, new_array.cell_array[i].octo_ship.RotationMatrix, { 0.75,0.75,0.0 });
		new_array.cell_array[i].octo_ship.move();
	}

	Draw_Object(window, shader_program, new_array_2.loaded_specs, lightPos, new_array_2.ScaleMatrix, new_array_2.TranslationMatrix, new_array_2.RotationMatrix, { 0.75,0.0,0.0 });

	for (int i = 0; i < new_array_2.num_current_cells; i++)
	{
		Draw_Object(window, shader_program, new_array.loaded_ship_specs, lightPos, new_array_2.cell_array[i].octo_ship.ScaleMatrix, new_array_2.cell_array[i].octo_ship.Transform_Matrix, new_array_2.cell_array[i].octo_ship.RotationMatrix, { 0.75,0.0,0.0 });
		new_array_2.cell_array[i].octo_ship.move();
	}
}

void Handle_Mouse_Click(double x_pos, double y_pos)
{
	for (int i = 0; i < new_array.num_current_cells; i++)
	{
		new_array.cell_array[i].octo_ship.set_new_orbit({x_pos, 0.0, y_pos});
	}
}


// Object Specific Functions

void Add_Ship_To_Manifest(Ship_Object* new_ship)
{
	ship_manifest.add_ship_to_manifest(new_ship);
}

void Remove_Ship_From_Manifest(Ship_Object* ship_to_remove)
{
	ship_manifest.remove_ship_from_manifest(ship_to_remove);
}