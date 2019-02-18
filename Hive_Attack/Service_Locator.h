#pragma once

class Gameplay_Manager;
class Render_Manager;
class Memory_Manager;

class Service_Locator
{
public:
	void register_service(Gameplay_Manager* gameplay_manager);
	void register_service(Render_Manager* render_manager);
	void register_service(Memory_Manager* memory_manager);

	Gameplay_Manager* return_gameplay_manager();
	Render_Manager* return_render_manager();
	Memory_Manager* return_memory_manager();

private:
	Gameplay_Manager * gameplay_manager;
	Render_Manager* render_manager;
	Memory_Manager* memory_manager;
};