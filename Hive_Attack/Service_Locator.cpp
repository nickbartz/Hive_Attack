#include "Service_Locator.h"

void Service_Locator::register_service(Gameplay_Manager * _gameplay_manager)
{
	gameplay_manager = _gameplay_manager;
}

void Service_Locator::register_service(Render_Manager * _render_manager)
{
	render_manager = _render_manager;
}

void Service_Locator::register_service(Memory_Manager * _memory_manager)
{
	memory_manager = _memory_manager;
}

Gameplay_Manager * Service_Locator::return_gameplay_manager()
{
	return gameplay_manager;
}

Render_Manager * Service_Locator::return_render_manager()
{
	return render_manager;
}

Memory_Manager * Service_Locator::return_memory_manager()
{
	return memory_manager;
}

