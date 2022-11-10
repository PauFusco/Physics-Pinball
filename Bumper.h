#pragma once
#ifndef __BUMPER_H__
#define __BUMPER_H__

#include "Entity.h"
#include "p2Point.h"
#include "ModulePhysics.h"

// #include "Animation.h"

#include "SDL/include/SDL.h"

#define MAX_JUMP_SPEED 7.0f
#define MAX_VEL 4.0f
#define DAMP 0.9f

struct SDL_Texture;

class Bumper : public Entity
{
public:

	Bumper();

	virtual ~Bumper();

	bool Start();

	bool Update();

	bool CleanUp();

	// Current animation check
	// Animation* currentAnimation = nullptr;

public:

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Debug();

	//Player Physics Body
	PhysBody* pbody;

private:

	//Texture Variables
	SDL_Texture* texture;
	const char* texturePath;
	int width;
	int height;

	//FX-Sound Variables
	int hitFxId;
	const char* hitFxPath;

	//Position Variables
	iPoint spawn;

	//Reaction Variables
	float bounceforce;
};

#endif // __BUMPER_H__