#pragma once

class Service_Locator;
class Projectile;

class Memory_Manager
{
public:
	Memory_Manager();
	void Init(Service_Locator* service_locator);
	Projectile* new_projectile();

	void Deallocate_Projectile(Projectile* projectile_pointer);

private:
	Projectile* projectile_array;
	int max_array_pointer = 1;
	int total_projectiles = 0;

	Service_Locator * service_locator;
};

