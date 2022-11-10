#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "p2Point.h"
#include "ModuleInput.h"
#include "ModuleRender.h"

class PhysBody;

enum class EntityType
{
	BALL,
	BUMPER,
	PALLET,
	UNKNOWN
};

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

public:

	EntityType type;
	bool active = true;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;
	bool renderable = true;
};

#endif // __ENTITY_H__