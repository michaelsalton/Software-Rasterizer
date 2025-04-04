#pragma once

#include "MathHelper.h"

class Entity
{
public:
	enum SPACE {local = 0, world = 1};

private:
	Vector3 mPosition;
	float mRotation;
	bool mActive;
	Entity* mParent;

public:
	Entity(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	~Entity();

	void Position(Vector3 position);
	Vector3 Position(SPACE space = world);

	void Rotation(float rotation);
	float Rotation(SPACE space = world);

	void Active(bool active);
	bool Active();

	void Parent(Entity* parent);
	Entity* Parent();

	virtual void Update();
	virtual void Render();
};
