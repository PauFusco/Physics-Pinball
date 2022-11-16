#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

#include <string>

using namespace std;

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// Initialise all the internal class variables, at least to NULL pointer
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{
	// You should do some memory cleaning here, if required
}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Load textures
	ballTex = App->textures->Load("Wahssets/Textures/Waluigi_Ball.png");

	background = App->textures->Load("Wahssets/Textures/Waluigi_Pinball_Map.png");

	bonus_fx = App->audio->LoadFx("Wahssets/Audio/Coin_Sound.wav");

	death_fx = App->audio->LoadFx("Wahssets/Audio/Wah.wav");

	launch_fx = App->audio->LoadFx("Wahssets/Audio/Waluigi_Time.wav");


	App->fonts->Load(fontPath, fontOrder, 2);

	int Waluigi_Pinball_Map[66] = {
	454, 639,
	454, 345,
	474, 345,
	480, 338,
	480, 101,
	417, 30,
	347, 0,
	196, 0,
	196, 30,
	93, 75,
	47, 136,
	47, 274,
	134, 353,
	134, 401,
	93, 460,
	93, 577,
	140, 641,
	347, 641,
	393, 576,
	396, 462,
	356, 398,
	356, 354,
	440, 273,
	439, 136,
	388, 74,
	288, 30,
	320, 30,
	401, 68,
	451, 123,
	451, 278,
	422, 302,
	420, 639,
	454, 639
	};
	App->physics->CreateChain(0, 0, Waluigi_Pinball_Map, 65);

	int MapBumpR[14] = {
	332, 90,
	317, 105,
	358, 130,
	377, 170,
	382, 148,
	382, 130,
	353, 96
	};
	App->physics->CreateChain(0, 0, MapBumpR, 14);
	
	int MapBumpL[16] = {
	155, 91,
	131, 96,
	104, 130,
	104, 153,
	113, 172,
	127, 130,
	142, 120,
	177, 107
	};
	App->physics->CreateChain(0, 0, MapBumpL, 16);

	SetDespawnDetector();

	SetBumpers(180, 425, SCREEN_WIDTH / 15);
	SetBumpers(305, 425, SCREEN_WIDTH / 15);
	
	SetBumpers(195, 300, SCREEN_WIDTH / 15);
	SetBumpers(290, 300, SCREEN_WIDTH / 15);
	SetBumpers(290, 225, SCREEN_WIDTH / 15);
	SetBumpers(195, 225, SCREEN_WIDTH / 15);
	SetBumpers(245, 265, SCREEN_WIDTH / 15);
	
	SetBumpers(195, 570, SCREEN_WIDTH / 20);
	SetBumpers(285, 570, SCREEN_WIDTH / 20);
	
	SetBumpers(439, 228, SCREEN_WIDTH / 10);
	SetBumpers( 47, 228, SCREEN_WIDTH / 10);

	SetPallets();

	SetLauncherFloor();

	App->audio->PlayMusic("Wahssets/Audio/Waluigi_Theme.ogg");

	return ret;
}

bool ModuleSceneIntro::CleanUp()

{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(background, 0, 0);
	
	if (lifes < 0)
	{
		lifes = 3;
		highScore = score;
		score = 0;
	}

	
	string temp = to_string(highScore);
	highChar = temp.c_str();

	App->fonts->BlitText(0, 0, 0, highInd);
	App->fonts->BlitText(50, 0, 0, highChar);
	
	temp = to_string(score);
	scoreChar = temp.c_str();
	App->fonts->BlitText(0, 15, 0, scoreInd);
	App->fonts->BlitText(50, 15, 0, scoreChar);

	temp = to_string(lifes);
	lifesChar = temp.c_str();
	App->fonts->BlitText(35, 370, 0, lifesInd);
	App->fonts->BlitText(55, 370, 0, lifesChar);

	App->renderer->Blit(ballTex, 20, 367);

	Debug();
	
	if (canLaunch && App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		circles.getFirst()->data->body->ApplyLinearImpulse(b2Vec2(0, -4.0f), circles.getFirst()->data->body->GetPosition(), true);
		App->audio->PlayFx(launch_fx);
		canLaunch = false;
	}

	// If user presses SPACE, enable RayCast
	//if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	//{
	//	// Enable raycast mode
	//	ray_on = !ray_on;
	//
	//	// Origin point of the raycast is be the mouse current position now (will not change)
	//	ray.x = App->input->GetMouseX();
	//	ray.y = App->input->GetMouseY();
	//}

	if (spawn && circles.getFirst() == nullptr)
	{
		circles.add(App->physics->CreateCircle(437, 557, 7.5f));
		
		circles.getLast()->data->ctype = ColliderType::BALL;
		circles.getLast()->data->listener = this;

		spawn = false;
	}
	
	if (despawn)
	{
		circles.getFirst()->data->body->DestroyFixture(circles.getFirst()->data->body->GetFixtureList());
		circles.del(circles.getFirst());

		App->audio->PlayFx(death_fx);
		lifes--;
		despawn = false;
		spawn = true;
	}

	// Prepare for raycast ------------------------------------------------------
	
	// The target point of the raycast is the mouse current position (will change over game time)
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	// Total distance of the raycast reference segment
	int ray_hit = ray.DistanceTo(mouse);

	// Declare a vector. We will draw the normal to the hit surface (if we hit something)
	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------

	// Circles
	p2List_item<PhysBody*>* c = circles.getFirst();
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		SDL_Rect ballrect;
		ballrect.x = 0;
		ballrect.y = 0;
		ballrect.w = 15;
		ballrect.h = 15;
		App->renderer->Blit(ballTex, x, y, &ballrect, 1.0f, c->data->GetRotation());

		c = c->next;
	}

	// Raycasts -----------------
	if(ray_on == true)
	{
		// Compute the vector from the raycast origin up to the contact point (if we're hitting anything; otherwise this is the reference length)
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		// Draw a line from origin to the hit point (or reference length if we are not hitting anything)
		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		// If we are hitting something with the raycast, draw the normal vector to the contact point
		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)	m_joint->EnableMotor(true);
	else m_joint->EnableMotor(false);
	// Keep playing
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	// Play Audio FX on every collision, regardless of who is colliding
	switch (bodyA->ctype) {
	case ColliderType::BALL:
		switch (bodyB->ctype) {
		case ColliderType::BUMPER:
			ApplyVectorImpulse(bodyA, bodyB);
			score += 100;
			App->audio->PlayFx(bonus_fx);
			break;
		case ColliderType::WALL:
			despawn = true;
			canLaunch = true;
			break;
		case ColliderType::LAUNCHER:
			canLaunch = true;
			/*App->audio->PlayFx(launch_fx);*/
			break;
		}
	}
}

void ModuleSceneIntro::SetBumpers(int x, int y, int diameter)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* big_ball = App->physics->world->CreateBody(&body);

	// Create a big circle shape
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	// Create a fixture and associate the circle to it
	b2FixtureDef fixture;
	fixture.shape = &shape;

	// Add the ficture (plus shape) to the static body
	big_ball->CreateFixture(&fixture);

	PhysBody* ballin = new PhysBody();
	ballin->body = big_ball;
	big_ball->SetUserData(ballin);
	ballin->ctype = ColliderType::BUMPER;

	ballin->listener = this;
}

void ModuleSceneIntro::SetPallets()
{
	//body and fixture defs - the common parts
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;

	//two shapes
	b2PolygonShape boxShape;
	boxShape.SetAsBox(PIXEL_TO_METERS(10), PIXEL_TO_METERS(10));
	
	b2CircleShape circleShape;
	circleShape.m_radius = 0.5f;

	//make box a little to the left
	bodyDef1.position.Set(5, 10);
	fixtureDef.shape = &boxShape;
	b2Body* m_bodyA = App->physics->world->CreateBody(&bodyDef1);
	m_bodyA->CreateFixture(&fixtureDef);

	b2BodyDef bodyDef2;
	bodyDef1.type = b2_staticBody;

	//and circle a little to the right
	bodyDef2.position.Set(7, 7);
	fixtureDef.shape = &circleShape;
	b2Body* m_bodyB = App->physics->world->CreateBody(&bodyDef2);
	m_bodyB->CreateFixture(&fixtureDef);

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = m_bodyA;
	revoluteJointDef.bodyB = m_bodyB;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(2, 2);//the top right corner of the box
	revoluteJointDef.localAnchorB.Set(0, 0);//center of the circle

	revoluteJointDef.enableLimit = true;
	revoluteJointDef.lowerAngle = -45 * DEGTORAD;
	revoluteJointDef.upperAngle = 45 * DEGTORAD;

	revoluteJointDef.enableMotor = false;
	revoluteJointDef.maxMotorTorque = 50;
	revoluteJointDef.motorSpeed = -50;//90 degrees per second

	m_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

		
}

void ModuleSceneIntro::ApplyVectorImpulse(PhysBody* bodyA, PhysBody* bodyB)
{
	int bumpx, ballx, bumpy, bally;

	bodyB->GetPosition(bumpx, bumpy);
	bodyA->GetPosition(ballx, bally);

	b2Vec2 forceDir = b2Vec2((ballx - bumpx), (bally - bumpy));

	bodyA->body->SetLinearVelocity(b2Vec2(0, 0));

	bodyA->body->ApplyLinearImpulse(bumpImp * forceDir, bodyA->body->GetPosition(), true);

	App->audio->PlayFx(bonus_fx);
}

void ModuleSceneIntro::CreateBall(int x, int y)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* big_ball = App->physics->world->CreateBody(&body);

	// Create a big circle shape
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(7);

	// Create a fixture and associate the circle to it
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	// Add the ficture (plus shape) to the static body
	big_ball->CreateFixture(&fixture);

	ballbod = new PhysBody();
	ballbod->body = big_ball;
	big_ball->SetUserData(ballbod);
	ballbod->ctype = ColliderType::BALL;

	ballbod->listener = this;
}

void ModuleSceneIntro::SetDespawnDetector()
{
	int x = 0;
	int y = SCREEN_HEIGHT - 5;

	b2BodyDef base;
	base.type = b2_staticBody;
	base.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* baseBody = App->physics->world->CreateBody(&base);

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(SCREEN_WIDTH), PIXEL_TO_METERS(5));

	b2FixtureDef fixture;
	fixture.shape = &shape;

	baseBody->CreateFixture(&fixture);

	PhysBody* yo = new PhysBody();
	yo->body = baseBody;
	baseBody->SetUserData(&yo);
	yo->ctype = ColliderType::WALL;

	yo->listener = this;
	baseBody->SetUserData(yo);

}

void ModuleSceneIntro::DespawnBall()
{
	circles.getFirst()->data->body->DestroyFixture(circles.getFirst()->data->body->GetFixtureList());
	circles.del(circles.getFirst());
}

void ModuleSceneIntro::SetLauncherFloor()
{
	int x = 438;
	int y = 582;

	b2BodyDef launcher;
	launcher.type = b2_staticBody;
	launcher.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* launcherBody = App->physics->world->CreateBody(&launcher);

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(23), PIXEL_TO_METERS(9));

	b2FixtureDef fixture;
	fixture.shape = &shape;

	launcherBody->CreateFixture(&fixture);

	PhysBody* yo = new PhysBody();
	yo->body = launcherBody;
	launcherBody->SetUserData(&yo);
	yo->ctype = ColliderType::LAUNCHER;

	yo->listener = this;
	launcherBody->SetUserData(yo);
}

void ModuleSceneIntro::Debug()
{
	if (App->input->GetKey(SDL_SCANCODE_F11)) bumpImp += 0.01f;
	if (App->input->GetKey(SDL_SCANCODE_F10)) bumpImp -= 0.01f;
}