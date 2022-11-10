#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "p2List.h"

class EntityManager : public Module
{
public:

	EntityManager(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~EntityManager();

	// Called after Awake
	bool Start();

	// Called every frame
	update_status Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);


public:

	p2List<Entity*> entities;

};

#endif // __ENTITYMANAGER_H__
