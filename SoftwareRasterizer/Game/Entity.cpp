#include "Entity.h"

Entity::Entity(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	mRotation = 0.0f;
	mActive = true;
	mParent = NULL;
}

Entity::~Entity()
{
	mParent = NULL;
}

void Entity::Position(Vector3 position)
{
	mPosition = position;
}

Vector3 Entity::Position(SPACE space)
{
	if (space == local || mParent == NULL)
	{
		return mPosition;
	}
	return mParent->Position(world) + RotateVector(mPosition, mParent->Rotation(local));
}

void Entity::Rotation(float rotation)
{
	mRotation = rotation;
	if (mRotation > 360.0f)
	{
		mRotation -= 360.0f;
	}
	if (mRotation < 0.0f)
	{
		mRotation += 360.0f;
	}
}

float Entity::Rotation(SPACE space)
{
	if (space == local || mParent == NULL)
	{
		return mRotation;
	}
	return mParent->Rotation(world) + mRotation;
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
	mPosition = Position(world) - parent->Position(world);
	mParent = parent;
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