#include<Memory_Manager.h>
#include<Service_Locator.h>
#include <stdio.h>   
#include <stdlib.h> 
#include<Object_Hive.h>
#include<Object_Hive_Swarm.h>
#include<Object_Hive_Ship.h>
#include<Object_Projectile.h>
#include<Object_Hive_Pod.h>

const int MAX_NUM_PROJECTILES = 1000;
const int MAX_NUM_SHIP_OBJECTS = 1000;
const int MAX_NUM_HIVE_POD_OBJECTS = 1000;
const int MAX_NUM_HIVES = 100;

Memory_Manager::Memory_Manager()
{

}

void Memory_Manager::Init(Service_Locator* _service_locator)
{
	projectile_array = new Projectile[MAX_NUM_PROJECTILES];
	ship_object_array = new Ship_Object[MAX_NUM_SHIP_OBJECTS];
	hive_pod_object_array = new Hive_Pod_Object[MAX_NUM_HIVE_POD_OBJECTS];
	hive_object_array = new Hive_Object[MAX_NUM_HIVES];

	service_locator = _service_locator;
}

Projectile* Memory_Manager::new_projectile()
{
	for (int i = 0; i < max_projectile_array_pointer; i++)
	{
		if (!projectile_array[i].is_init())
		{
			projectile_array[i] = Projectile();
			if (i + 1 == max_projectile_array_pointer) max_projectile_array_pointer++;
			total_projectiles++;
			projectile_array[i] = Projectile();
			return &projectile_array[i];
		}
	}
}

void Memory_Manager::Deallocate_Projectile(Projectile* projectile_pointer)
{
	for (int i = 0; i < max_projectile_array_pointer; i++)
	{
		if (&projectile_array[i] == projectile_pointer)
		{
			if (i + 1 == max_projectile_array_pointer) max_projectile_array_pointer--;
			projectile_pointer->Destroy();

			total_projectiles--;
		}
	}
}

Ship_Object * Memory_Manager::new_ship()
{
	for (int i = 0; i < max_ship_array_pointer; i++)
	{
		if (!ship_object_array[i].is_active())
		{
			if (i + 1 == max_ship_array_pointer) max_ship_array_pointer++;
			total_ships++;
			ship_object_array[i] = Ship_Object();
			return &ship_object_array[i];
		}
	}
}

void Memory_Manager::Deallocate_Ship(Ship_Object * ship_pointer)
{
	for (int i = 0; i < max_ship_array_pointer; i++)
	{
		if (&ship_object_array[i] == ship_pointer)
		{
			if (i + 1 == max_ship_array_pointer) max_ship_array_pointer--;
			ship_pointer->destroy();
			total_ships--;
		}
	}
}

Hive_Pod_Object * Memory_Manager::new_hive_pod_object()
{
	for (int i = 0; i < max_hive_pod_object_pointer; i++)
	{
		if (!hive_pod_object_array[i].is_active())
		{
			if (i + 1 == max_hive_pod_object_pointer) max_hive_pod_object_pointer++;
			total_hive_pod_objects++;
			hive_pod_object_array[i] = Hive_Pod_Object();
			return &hive_pod_object_array[i];
		}
	}
}

void Memory_Manager::Deallocate_Hive_Pod_Object(Hive_Pod_Object * hive_pod_pointer)
{
	for (int i = 0; i < max_hive_pod_object_pointer; i++)
	{
		if (&hive_pod_object_array[i] == hive_pod_pointer)
		{
			if (i + 1 == max_hive_pod_object_pointer) max_hive_pod_object_pointer--;
			hive_pod_pointer->destroy();
			total_hive_pod_objects--;
		}
	}
}

Hive_Object * Memory_Manager::new_hive_object()
{
	// THIS IS BROKEN FOR NOW, NEEDS TO BE LINKED TO A SWATH OF MEMORY GRABBED AT STARTUP
	return new Hive_Object;
}

void Memory_Manager::Deallocate_Hive_Object(Hive_Object * hive_pointer)
{
	for (int i = 0; i < max_hive_object_pointer; i++)
	{
		if (&hive_object_array[i] == hive_pointer)
		{
			if (i + 1 == max_hive_object_pointer) max_hive_object_pointer--;
			hive_pointer->destroy();
			hive_object_array[i] = Hive_Object();
			total_hive_objects--;
		}
	}
}

Hive_Ship_Array * Memory_Manager::new_hive_ship_array()
{
	return new Hive_Ship_Array;
}

void Memory_Manager::Deallocate_Hive_Ship_Array(Hive_Ship_Array * hive_ship_array)
{
}
