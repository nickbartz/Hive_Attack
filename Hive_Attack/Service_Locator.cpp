#include <Service_Locator.h>


Global_Service_Locator::Global_Service_Locator()
{

}

UI * Global_Service_Locator::get_UI_pointer()
{
	return UI_pointer;
}

Message_Array* Global_Service_Locator::get_MB_Pointer()
{
	return msg_bus_pointer;
}

Draw_System* Global_Service_Locator::get_Draw_System_Pointer()
{
	return draw_system_pointer;
}

Cursor* Global_Service_Locator::get_Cursor_Pointer()
{
	return cursor_pointer;
}

Game_Library* Global_Service_Locator::get_Game_Library()
{
	return game_library;
}

Scene_Graph* Global_Service_Locator::get_Scene_Graph()
{
	return scene_pointer;
}

Path_Field* Global_Service_Locator::get_Pathfinder()
{
	return pathfinder_pointer;
}

void Global_Service_Locator::Register_UI_Pointer(UI* ui_pointer)
{
	UI_pointer = ui_pointer;
}

void Global_Service_Locator::Register_MB_Pointer(Message_Array* msg_pointer)
{
	msg_bus_pointer = msg_pointer;
}

void Global_Service_Locator::Register_Draw_System_Pointer(Draw_System* draw_pointer)
{
	draw_system_pointer = draw_pointer;
}

void Global_Service_Locator::Register_Cursor_Pointer(Cursor* cPointer)
{
	cursor_pointer = cPointer;
}

void Global_Service_Locator::Register_Game_Library(Game_Library* gLibrary)
{
	game_library = gLibrary;
}


void Global_Service_Locator::Register_Scene_Graph(Scene_Graph* sGraph)
{
	scene_pointer = sGraph;
}

void Global_Service_Locator::Register_Pathfinder(Path_Field* pFind)
{
	pathfinder_pointer = pFind;
}

void Global_Service_Locator::free()
{
}

void Object_Service_Locator::Register_Pointer(Object* oPointer)
{
	object_pointer = oPointer;
}

void Object_Service_Locator::Register_Pointer(Render_Component* sPointer)
{
	Render_Pointer = sPointer;
}

void Object_Service_Locator::Register_Pointer(AI_Stats_Component* bPointer)
{
	AI_Stats_Pointer = bPointer;
}

void Object_Service_Locator::Register_Pointer(AI_Movement_Component* mPointer)
{
	AI_Movement_Pointer = mPointer;
}

void Object_Service_Locator::Register_Pointer(AI_Rel_Component* rPointer)
{
	AI_Rel_Pointer = rPointer;
}

void Object_Service_Locator::Register_Pointer(AI_Item_Component* iPointer)
{
	AI_Item_Pointer = iPointer;
}

void Object_Service_Locator::Register_Pointer(AI_Job_Component* jPointer)
{
	AI_Job_Pointer = jPointer;
}

Object* Object_Service_Locator::Return_Object_Pointer()
{
	return object_pointer;
}

Render_Component* Object_Service_Locator::Return_Render_Pointer()
{
	return Render_Pointer;
}

AI_Stats_Component* Object_Service_Locator::Return_AI_Stats_Pointer()
{
	return AI_Stats_Pointer;
}

AI_Movement_Component* Object_Service_Locator::Return_AI_Movement_Pointer()
{
	return AI_Movement_Pointer;
}

AI_Rel_Component* Object_Service_Locator::Return_AI_Rel_Pointer()
{
	return AI_Rel_Pointer;
}

AI_Item_Component* Object_Service_Locator::Return_AI_Item_Pointer()
{
	return AI_Item_Pointer;
}

AI_Job_Component* Object_Service_Locator::Return_AI_Job_Pointer()
{
	return AI_Job_Pointer;
}