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

	int PalletBodyL[8] = {
		121, 533,
		125, 524,
		168, 613,
		159, 620
	};
	App->physics->CreateChain(0, 0, PalletBodyL, 8);
	
	int PalletBodyR[8] = {
		361, 524,
		365, 533,
		327, 621,
		318, 613
	};
	App->physics->CreateChain(0, 0, PalletBodyR, 8);
	
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
	
	SetBumpers(439, 230, SCREEN_WIDTH / 9);
	SetBumpers( 45, 233, SCREEN_WIDTH / 8);

	SetPalletR();
	SetPalletL();

	SetLauncherFloor();

	//App->audio->PlayMusic("Wahssets/Audio/Waluigi_Theme.ogg");

	return ret;
}

bool ModuleSceneIntro::CleanUp()

{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update()
{
	if (renderBack)	App->renderer->Blit(background, 0, 0);
	
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

	// Spawn in launcher
	if (spawn && circles.getFirst() == nullptr)
	{
		circles.add(App->physics->CreateCircle(437, 557, 7.5f));
		
		circles.getLast()->data->ctype = ColliderType::BALL;
		circles.getLast()->data->listener = this;

		spawn = false;
	}
	
	// Launch
	if (canLaunch && App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		circles.getFirst()->data->body->ApplyLinearImpulse(b2Vec2(0, -4.0f), circles.getFirst()->data->body->GetPosition(), true);
		App->audio->PlayFx(launch_fx);
		canLaunch = false;
	}
	
	// diE
	if (despawn)
	{
		circles.getFirst()->data->body->DestroyFixture(circles.getFirst()->data->body->GetFixtureList());
		circles.del(circles.getFirst());

		App->audio->PlayFx(death_fx);
		lifes--;
		despawn = false;
		spawn = true;
	}

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

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) m_jointR->EnableMotor(true);
	else m_jointR->EnableMotor(false);
	
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) m_jointL->EnableMotor(true);
	else m_jointL->EnableMotor(false);
	
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

		case ColliderType::PALLET:
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

void ModuleSceneIntro::SetPalletR()
{
	int x, y;
	x = 320, y = 620;

	//body and fixture defs - the common parts
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;

	//two shapes
	b2PolygonShape boxShapebig;
	boxShapebig.SetAsBox(PIXEL_TO_METERS(25), PIXEL_TO_METERS(5));

	b2PolygonShape boxShapesmol;
	boxShapesmol.SetAsBox(PIXEL_TO_METERS(5), PIXEL_TO_METERS(2));


	//make box a little to the left
	bodyDef1.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	fixtureDef.shape = &boxShapebig;
	b2Body* m_bodyA = App->physics->world->CreateBody(&bodyDef1);
	m_bodyA->CreateFixture(&fixtureDef);

	b2BodyDef bodyDef2;
	bodyDef1.type = b2_staticBody;

	bodyDef2.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	fixtureDef.shape = &boxShapesmol;

	b2Body* m_bodyB = App->physics->world->CreateBody(&bodyDef2);
	m_bodyB->CreateFixture(&fixtureDef);

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = m_bodyA;
	revoluteJointDef.bodyB = m_bodyB;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(0.5f, 0);//the top right corner of the box
	revoluteJointDef.localAnchorB.Set(-0.1, 0);//center of the box

	revoluteJointDef.enableLimit = true;
	revoluteJointDef.lowerAngle = -45 * DEGTORAD;
	revoluteJointDef.upperAngle = 10 * DEGTORAD;

	revoluteJointDef.enableMotor = false;
	revoluteJointDef.maxMotorTorque = 1000;
	revoluteJointDef.motorSpeed = -5;//90 degrees per second

	m_jointR = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	PhysBody* base = new PhysBody();
	PhysBody* pallet = new PhysBody();
	base->body = m_bodyA;
	pallet->body = m_bodyB;

	m_bodyA->SetUserData(base);
	m_bodyB->SetUserData(pallet);

	base->ctype = ColliderType::PALLET;
	pallet->ctype = ColliderType::PALLET;

//int PalletBodyR[8] = {
	//	361, 524,
	//	365, 533,
	//	327, 621,
	//	318, 613
	//};
	//b2BodyDef body;
	//body.type = b2_staticBody;
	//body.position.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	//
	//// Add BODY to the world
	//b2Body* b = App->physics->world->CreateBody(&body);
	//
	//// Create SHAPE
	//b2ChainShape shape;
	//b2Vec2* p = new b2Vec2[8 / 2];
	//for (uint i = 0; i < 8 / 2; ++i)
	//{
	//	p[i].x = PIXEL_TO_METERS(PalletBodyR[i * 2 + 0]);
	//	p[i].y = PIXEL_TO_METERS(PalletBodyR[i * 2 + 1]);
	//}
	//shape.CreateLoop(p, 8 / 2);
	//
	//// Create FIXTURE
	//b2FixtureDef fixture;
	//fixture.shape = &shape;
	//fixture.friction = 0.1f;
	//fixture.density = 1;
	//
	//// Add fixture to the BODY
	//b->CreateFixture(&fixture);
	//
	//// Clean-up temp array
	//delete p;
	//
	//// Create our custom PhysBody class
	//PhysBody* pbody = new PhysBody();
	//pbody->body = b;
	//b->SetUserData(pbody);
	//pbody->width = pbody->height = 0;
	//pbody->ctype = ColliderType::WALL;
	//// ---------------------------------------------------------
	//
	//int PalasR[10] = {
	//	116, 0,
	//	168, 4,
	//	170, 10,
	//	166, 13,
	//	93, 0
	//};
	//b2BodyDef bodyb;
	//bodyb.type = b2_dynamicBody;
	//bodyb.position.Set(PIXEL_TO_METERS(145), PIXEL_TO_METERS(607));
	//
	//// Add BODY to the world
	//b2Body* bb = App->physics->world->CreateBody(&bodyb);
	//
	//// Create SHAPE
	//b2ChainShape shapeb;
	//p = new b2Vec2[10 / 2];
	//for (uint i = 0; i < 10 / 2; ++i)
	//{
	//	p[i].x = PIXEL_TO_METERS(PalasR[i * 2 + 0]);
	//	p[i].y = PIXEL_TO_METERS(PalasR[i * 2 + 1]);
	//}
	//shapeb.CreateLoop(p, 10 / 2);
	//
	//// Create FIXTURE
	//b2FixtureDef fixtureb;
	//fixtureb.shape = &shapeb;
	//fixtureb.friction = 0.1f;
	//fixture.density = 1;
	//
	//// Add fixture to the BODY
	//bb->CreateFixture(&fixtureb);
	//
	//// Clean-up temp array
	//delete p;
	//
	//// Create our custom PhysBody class
	//PhysBody* pbodyb = new PhysBody();
	//pbodyb->body = bb;
	//bb->SetUserData(pbodyb);
	//pbodyb->width = pbodyb->height = 0;
	//pbodyb->ctype = ColliderType::PALLET;
	//// Return our PhysBody class
	//
	//b2RevoluteJointDef revoluteJointDef;
	//revoluteJointDef.bodyA = b;
	//revoluteJointDef.bodyB = bb;
	//revoluteJointDef.collideConnected = false;
	//revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(130), PIXEL_TO_METERS(635));
	//revoluteJointDef.localAnchorB.Set(-0.5f, 0);
	//
	//revoluteJointDef.enableLimit = true;
	//revoluteJointDef.lowerAngle = -360 * DEGTORAD;
	//revoluteJointDef.upperAngle = 360 * DEGTORAD;
	//
	//revoluteJointDef.enableMotor = true;
	//revoluteJointDef.maxMotorTorque = 1000000000;
	//revoluteJointDef.motorSpeed = 5;//90 degrees per second
	//
	//m_joint = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);
	//
	//PhysBody* base = new PhysBody();
	//PhysBody* pallet = new PhysBody();
	//base->body = b;
	//pallet->body = bb;
	//
	//base->ctype = ColliderType::WALL;
	//pallet->ctype = ColliderType::PALLET;
}

void ModuleSceneIntro::SetPalletL()
{
	int x, y;
	x = 165, y = 620;

	//body and fixture defs - the common parts
	b2BodyDef bodyDef1;
	bodyDef1.type = b2_dynamicBody;
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;

	//two shapes
	b2PolygonShape boxShapebig;
	boxShapebig.SetAsBox(PIXEL_TO_METERS(25), PIXEL_TO_METERS(5));
	
	b2PolygonShape boxShapesmol;
	boxShapesmol.SetAsBox(PIXEL_TO_METERS(5), PIXEL_TO_METERS(2));
	
	//make box a little to the left
	bodyDef1.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	fixtureDef.shape = &boxShapebig;
	b2Body* m_bodyA = App->physics->world->CreateBody(&bodyDef1);
	m_bodyA->CreateFixture(&fixtureDef);

	b2BodyDef bodyDef2;
	bodyDef2.type = b2_staticBody;


	bodyDef2.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	fixtureDef.shape = &boxShapesmol;

	b2Body* m_bodyB = App->physics->world->CreateBody(&bodyDef2);
	m_bodyB->CreateFixture(&fixtureDef);

	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = m_bodyA;
	revoluteJointDef.bodyB = m_bodyB;
	revoluteJointDef.collideConnected = false;
	revoluteJointDef.localAnchorA.Set(-0.5f, 0);//the top right corner of the box
	revoluteJointDef.localAnchorB.Set(0.1, 0);//center of the box

	revoluteJointDef.enableLimit = true;
	revoluteJointDef.lowerAngle = -10 * DEGTORAD;
	revoluteJointDef.upperAngle = 45 * DEGTORAD;

	revoluteJointDef.enableMotor = false;
	revoluteJointDef.maxMotorTorque = 1000;
	revoluteJointDef.motorSpeed = 5;//90 degrees per second

	m_jointL = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	PhysBody* base = new PhysBody();
	PhysBody* pallet = new PhysBody();
	base->body = m_bodyA;
	pallet->body = m_bodyB;

	m_bodyA->SetUserData(base);
	m_bodyB->SetUserData(pallet);

	base->ctype = ColliderType::PALLET;
	pallet->ctype = ColliderType::PALLET;
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
	int y = SCREEN_HEIGHT+5;

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
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) bumpImp += 0.01f;
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) bumpImp -= 0.01f;

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) renderBack = !renderBack;
}