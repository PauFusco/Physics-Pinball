#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class b2RevoluteJoint;

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
	void SetPalletR();
	void SetPalletL();
	void ApplyVectorImpulse(PhysBody* bodyA, PhysBody* bodyB);
	void CreateBall(int x, int y);
	void SetDespawnDetector();
	void DespawnBall();
	void SetLauncherFloor();
	void Debug();

public:
	p2List<PhysBody*> circles;

	PhysBody* back;

	// Lower ground sensor (will kill all objects passig through it)
	PhysBody* despawn_sensor;
	bool sensed;

	PhysBody* ballbod;

	// Textures
	SDL_Texture* ballTex;
	SDL_Texture* background;
	SDL_Texture* pallets;

	// FX
	uint bonus_fx;
	uint death_fx;
	uint launch_fx;

	// Raycast
	p2Point<int> ray;
	bool ray_on;

	int lifes = 3;

	// Points
	int score = 0;
	int highScore = 0;
	int prevScore = 0;
	
	// Points as char
	char* scoreInd = "SCORE ";
	char* highInd =	 "HIGH ";
	char* lifesInd = " X ";
	char* prevInd = "PREV ";
	
	// Point text
	const char* highChar;
	const char* scoreChar;
	const char* lifesChar;
	const char* prevChar;

	b2RevoluteJoint* m_jointR;
	b2RevoluteJoint* m_jointL;

	bool despawn = false;
	bool spawn = true;
	bool canLaunch = true;

	float bumpImp = 0.015f;

	char* fontPath = "Wahssets/Fonts/rtype_font3.png";
	char* fontOrder = "! c,_./0123456789e;<>?ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	bool renderBack = true;
};
