#pragma once

class Service_Locator;
class Projectile;
class Ship_Object;

class Memory_Manager
{
public:
	Memory_Manager();
	void Init(Service_Locator* service_locator);

	Projectile* new_projectile();
	void Deallocate_Projectile(Projectile* projectile_pointer);

	Ship_Object* new_ship();
	void Deallocate_Ship(Ship_Object* ship_pointer);

private:
	Projectile* projectile_array;
	int max_projectile_array_pointer = 1;
	int total_projectiles = 0;

	Ship_Object* ship_object_array;
	int max_ship_array_pointer = 1;
	int total_ships = 0;

	Service_Locator * service_locator;
};

