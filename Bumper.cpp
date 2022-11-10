#include "Bumper.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "p2Point.h"
#include "ModulePhysics.h"

Bumper::Bumper() : Entity(EntityType::BUMPER)
{}

Bumper::~Bumper()
{}

bool Bumper::Start()
{
	bool ret = true;
	
	//

	return ret;
}

bool Bumper::Update()
{
	bool ret = true;

	//

	return ret;
}

bool Bumper::CleanUp()
{
	bool ret = true;

	// Clean all bumpers

	return ret;
}

void Bumper::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::BALL:
		
		break;
	}

}