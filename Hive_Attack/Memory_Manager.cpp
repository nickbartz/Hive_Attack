#include<Memory_Manager.h>
#include<Service_Locator.h>
#include <stdio.h>   
#include <stdlib.h> 
#include<Object_Static.h>

const int MAX_NUM_PROJECTILES = 1000;

Memory_Manager::Memory_Manager()
{

}

void Memory_Manager::Init(Service_Locator* _service_locator)
{
	projectile_array = new Projectile[MAX_NUM_PROJECTILES];

	service_locator = _service_locator;
}

Projectile* Memory_Manager::new_projectile()
{
	for (int i = 0; i < max_array_pointer; i++)
	{
		if (!projectile_array[i].is_init())
		{
			if (i + 1 == max_array_pointer) max_array_pointer++;
			total_projectiles++;
			return &projectile_array[i];
		}
	}
}

void Memory_Manager::Deallocate_Projectile(Projectile* projectile_pointer)
{
	for (int i = 0; i < max_array_pointer; i++)
	{
		if (&projectile_array[i] == projectile_pointer)
		{
			if (i + 1 == max_array_pointer) max_array_pointer--;
			projectile_pointer->Destroy();
			total_projectiles--;
		}
	}
}