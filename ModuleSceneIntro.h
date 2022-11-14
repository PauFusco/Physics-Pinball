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
	void ApplyVectorImpulse(PhysBody* bodyA, PhysBody* bodyB);
	void CreateBall(int x, int y);
	void SetDespawnDetector();
	void DespawnBall();
	void SetLauncherFloor();

public:

	p2List<PhysBody*> circles;
	p2List<PhysBody*> ricks;

	PhysBody* back;

	// Lower ground sensor (will kill all objects passig through it)
	PhysBody* despawn_sensor;
	bool sensed;

	PhysBody* ballbod;

	// Textures
	SDL_Texture* ballTex;
	SDL_Texture* background;

	// FX
	uint bonus_fx;

	// Raycast
	p2Point<int> ray;
	bool ray_on;

	int lifes;

	bool despawn = false;
	bool spawn = true;
};
