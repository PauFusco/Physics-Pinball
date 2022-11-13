#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:

	// Constructors & Destructors
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	// Main module steps
	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void SetBumpers(int x, int y, int diameter);
	void SetPallets();

public:

	// Lists of physics objects
	p2List<PhysBody*> circles;
	//p2List<PhysBody*> boxes;
	//p2List<PhysBody*> ricks;

	// Lower ground sensor (will kill all objects passig through it)
	PhysBody* lower_ground_sensor;
	PhysBody* higher_ground_sensor;
	PhysBody* right_ground_sensor;
	PhysBody* left_ground_sensor;
	bool sensed;

	PhysBody* ball;

	// Textures
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;

	// FX
	uint bonus_fx;

	// Raycast
	p2Point<int> ray;
	bool ray_on;
};
