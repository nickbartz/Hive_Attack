#pragma once
#include<Object_Hive_Swarm.h>

class Projectile
{
public:
	Projectile();
	void Init(vec3 start, vec3 finish);
	void Destroy();
	void update();
	void update_transform_matrix(glm::vec3 transform_vector);
	void Draw();
	void Decrement_Lifespan();
	int Return_Lifespan();
	mat4 Return_Model_Matrix();
	vec3 Return_Color();
	bool is_init();

private:
	int lifespan;

	vec3 start_position;
	vec3 end_position;
	vec3 current_transform_vector;
	vec3 direction_vector;

	mat4 ScaleMatrix = scale(vec3{ 0.1f,0.1f,0.1f });
	mat4 Transform_Matrix = glm::mat4(1.0);
	mat4 RotationMatrix = glm::mat4(1.0);

	float speed;

	bool initialized = false;

	vec3 color;
};

