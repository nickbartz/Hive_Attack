#pragma once

class Service_Locator;
class Projectile;
class Ship_Object;
class Hive_Pod_Object;
class Hive_Object;

class Memory_Manager
{
public:
	Memory_Manager();
	void Init(Service_Locator* service_locator);

	Projectile* new_projectile();
	void Deallocate_Projectile(Projectile* projectile_pointer);

	Ship_Object* new_ship();
	void Deallocate_Ship(Ship_Object* ship_pointer);

	Hive_Pod_Object* new_hive_pod_object();
	void Deallocate_Hive_Pod_Object(Hive_Pod_Object* hive_pod_pointer);

	Hive_Object* new_hive_object();
	void Deallocate_Hive_Object(Hive_Object* hive_pointer);

private:
	Projectile* projectile_array;
	int max_projectile_array_pointer = 1;
	int total_projectiles = 0;

	Ship_Object* ship_object_array;
	int max_ship_array_pointer = 1;
	int total_ships = 0;

	Hive_Pod_Object* hive_pod_object_array;
	int max_hive_pod_object_pointer = 1;
	int total_hive_pod_objects = 0;

	Hive_Object* hive_object_array;
	int max_hive_object_pointer = 1;
	int total_hive_objects = 0;

	Service_Locator * service_locator;
};

