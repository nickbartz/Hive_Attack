#include<Object_Projectile.h>

Projectile::Projectile()
{

}

void Projectile::Init(vec3 start, vec3 finish)
{
	start_position = start;
	end_position = finish;
	current_transform_vector = start_position;
	direction_vector = end_position - start_position;
	speed = 0.1;

	float distance = sqrt(direction_vector.x*direction_vector.x + direction_vector.z*direction_vector.z);

	lifespan = distance / speed;
	initialized = true;

	color = { 1.0f,1.0f,1.0f };
}

void Projectile::Draw()
{

}

void Projectile::Decrement_Lifespan()
{
	lifespan--;

	if (lifespan <= 0) lifespan = 0;
}

int Projectile::Return_Lifespan()
{
	return lifespan;
}

mat4 Projectile::Return_Model_Matrix()
{
	return Transform_Matrix * RotationMatrix*ScaleMatrix;
}

vec3 Projectile::Return_Color()
{
	return color;
}

bool Projectile::is_init()
{
	return initialized;
}

void Projectile::Destroy()
{
	initialized = false;
}

void Projectile::update()
{
	current_transform_vector += normalize(direction_vector) * speed;
	update_transform_matrix(current_transform_vector);

	Decrement_Lifespan();
}

void Projectile::update_transform_matrix(glm::vec3 transform_vector)
{
	Transform_Matrix = glm::translate(transform_vector);
	current_transform_vector = transform_vector;
}