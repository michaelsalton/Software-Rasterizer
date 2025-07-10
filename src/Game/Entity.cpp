#include "Game/Entity.h"

Entity::Entity(float x, float y, float z)
{
	transform.setPosition(x, y, z);
	mActive = true;
	mParent = NULL;
}

Entity::~Entity()
{
	mParent = NULL;
}

void Entity::Position(Vec3 position)
{
	transform.setPosition(position);
}

Vec3 Entity::Position(SPACE space)
{
	if (space == local || mParent == NULL)
	{
		return transform.getPosition();
	}
	return transform.getWorldPosition();
}

void Entity::Rotation(float rotation)
{
	// Convert single float rotation (degrees) to Euler angles
	transform.setRotation(0, Math::toRadians(rotation), 0);
}

float Entity::Rotation(SPACE space)
{
	Vec3 rot = (space == local || mParent == NULL) 
		? transform.getRotation() 
		: transform.getWorldMatrix().toEulerAngles();
	
	// Return Y rotation in degrees
	return Math::toDegrees(rot.y);
}

void Entity::Scale(const Vec3& scale)
{
	transform.setScale(scale);
}

void Entity::Scale(float scale)
{
	transform.setScale(scale);
}

Vec3 Entity::Scale(SPACE space)
{
	if (space == local || mParent == NULL)
	{
		return transform.getScale();
	}
	
	// Extract scale from world matrix
	Mat4 worldMat = transform.getWorldMatrix();
	Vec3 translation, scale;
	Mat4 rotation;
	worldMat.decompose(translation, rotation, scale);
	return scale;
}

Mat4 Entity::GetModelMatrix() const
{
	return transform.getModelMatrix();
}

Mat4 Entity::GetWorldMatrix() const
{
	return transform.getWorldMatrix();
}

void Entity::Active(bool active)
{
	mActive = active;
}

bool Entity::Active()
{
	return mActive;
}

void Entity::Parent(Entity* parent)
{
	mParent = parent;
	if (parent)
	{
		transform.setParent(&parent->transform);
	}
	else
	{
		transform.setParent(nullptr);
	}
}

Entity* Entity::Parent()
{
	return mParent;
}

void Entity::Update()
{
}

void Entity::Render()
{
}