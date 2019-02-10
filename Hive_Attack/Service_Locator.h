#pragma once

class UI;
class Message_Array;
class Draw_System;
class Cursor;
class Game_Library;
class Scene_Graph;
class Path_Field;

class Global_Service_Locator
{
public:
	Global_Service_Locator();

	UI * get_UI_pointer();
	Message_Array* get_MB_Pointer();
	Draw_System* get_Draw_System_Pointer();
	Cursor* get_Cursor_Pointer();
	Game_Library* get_Game_Library();
	Scene_Graph* get_Scene_Graph();
	Path_Field* get_Pathfinder();


	void Register_UI_Pointer(UI* ui_pointer);
	void Register_MB_Pointer(Message_Array* msg_pointer);
	void Register_Draw_System_Pointer(Draw_System* draw_pointer);
	void Register_Cursor_Pointer(Cursor* cursor_pointer);
	void Register_Game_Library(Game_Library* game_library);
	void Register_Scene_Graph(Scene_Graph* sGraph);
	void Register_Pathfinder(Path_Field* pFind);

	void free();

private:
	UI * UI_pointer;
	Message_Array* msg_bus_pointer;
	Draw_System* draw_system_pointer;
	Cursor* cursor_pointer;
	Game_Library* game_library;
	Scene_Graph* scene_pointer;
	Path_Field* pathfinder_pointer;
};

class Render_Component;
class AI_Stats_Component;
class AI_Movement_Component;
class AI_Rel_Component;
class AI_Item_Component;
class AI_Job_Component;
class Object;

class Object_Service_Locator
{
public:
	void Register_Pointer(Object* Object);
	void Register_Pointer(Render_Component* Simple_Render_Pointer);
	void Register_Pointer(AI_Stats_Component* Basic_Structure_AI_Pointer);
	void Register_Pointer(AI_Movement_Component* AI_Movement_Pointer);
	void Register_Pointer(AI_Rel_Component* AI_Rel_Pointer);
	void Register_Pointer(AI_Item_Component* AI_Item_Pointer);
	void Register_Pointer(AI_Job_Component* AI_Job_Pointer);

	Object* Return_Object_Pointer();
	Render_Component* Return_Render_Pointer();
	AI_Stats_Component* Return_AI_Stats_Pointer();
	AI_Movement_Component* Return_AI_Movement_Pointer();
	AI_Rel_Component* Return_AI_Rel_Pointer();
	AI_Item_Component* Return_AI_Item_Pointer();
	AI_Job_Component* Return_AI_Job_Pointer();

private:
	Object * object_pointer;
	Render_Component * Render_Pointer;
	AI_Stats_Component* AI_Stats_Pointer;
	AI_Movement_Component* AI_Movement_Pointer;
	AI_Rel_Component* AI_Rel_Pointer;
	AI_Item_Component* AI_Item_Pointer;
	AI_Job_Component* AI_Job_Pointer;
};