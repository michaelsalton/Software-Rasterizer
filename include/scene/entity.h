#pragma once

#include "math/math.h"
#include "core/transform.h"

class Entity
{
public:
	enum SPACE {local = 0, world = 1};

protected:
	Transform transform;
	bool mActive;
	Entity* mParent;

public:
	Entity(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	~Entity();

	// Transform access
	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }

	// Legacy position/rotation interface (for compatibility)
	void Position(Vec3 position);
	Vec3 Position(SPACE space = world);

	void Rotation(float rotation);
	float Rotation(SPACE space = world);

	// Scale access
	void Scale(const Vec3& scale);
	void Scale(float scale);
	Vec3 Scale(SPACE space = world);

	// Model matrix access
	Mat4 GetModelMatrix() const;
	Mat4 GetWorldMatrix() const;

	void Active(bool active);
	bool Active();

	void Parent(Entity* parent);
	Entity* Parent();

	virtual void Update();
	virtual void Render();
};
