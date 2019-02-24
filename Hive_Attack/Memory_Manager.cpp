#include<Memory_Manager.h>
#include<Service_Locator.h>
#include <stdio.h>   
#include <stdlib.h> 
#include<Object_Static.h>
#include<Object_Dynamic.h>

const int MAX_NUM_PROJECTILES = 1000;
const int MAX_NUM_SHIP_OBJECTS = 1000;

Memory_Manager::Memory_Manager()
{

}

void Memory_Manager::Init(Service_Locator* _service_locator)
{
	projectile_array = new Projectile[MAX_NUM_PROJECTILES];
	ship_object_array = new Ship_Object[MAX_NUM_SHIP_OBJECTS];

	service_locator = _service_locator;
}

Projectile* Memory_Manager::new_projectile()
{
	for (int i = 0; i < max_projectile_array_pointer; i++)
	{
		if (!projectile_array[i].is_init())
		{
			if (i + 1 == max_projectile_array_pointer) max_projectile_array_pointer++;
			total_projectiles++;
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
