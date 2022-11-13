#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

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
	ball = App->textures->Load("Wahssets/Textures/Waluigi_Ball.png");

	background = App->textures->Load("Wahssets/Textures/Waluigi_Pinball_Map.png");

	bonus_fx = App->audio->LoadFx("Wahssets/Audio/bonus.wav");

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	SetDespawnDetector();

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()

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
	SetBumpers(47, 228, SCREEN_WIDTH / 10);

	SetPallets();

	CreateBall(SCREEN_WIDTH / 4,0 );

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
	App->renderer->Blit(background, 0, 0);
	// If user presses SPACE, enable RayCast
	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		// Enable raycast mode
		ray_on = !ray_on;

		// Origin point of the raycast is be the mouse current position now (will not change)
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	// If user presses 1, create a new circle object
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 7));
	
		// Add this module (ModuleSceneIntro) as a "listener" interested in collisions with circles.
		// If Box2D detects a collision with this last generated circle, it will automatically callback the function ModulePhysics::BeginContact()
		circles.getLast()->data->ctype = ColliderType::BALL;
		
		circles.getLast()->data->listener = this;
	}

	//// If user presses 3, create a new RickHead object
	//if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	//{
	//	// Pivot 0, 0
	//	int rick_head[64] = {
	//		14, 36,
	//		42, 40,
	//		40, 0,
	//		75, 30,
	//		88, 4,
	//		94, 39,
	//		111, 36,
	//		104, 58,
	//		107, 62,
	//		117, 67,
	//		109, 73,
	//		110, 85,
	//		106, 91,
	//		109, 99,
	//		103, 104,
	//		100, 115,
	//		106, 121,
	//		103, 125,
	//		98, 126,
	//		95, 137,
	//		83, 147,
	//		67, 147,
	//		53, 140,
	//		46, 132,
	//		34, 136,
	//		38, 126,
	//		23, 123,
	//		30, 114,
	//		10, 102,
	//		29, 90,
	//		0, 75,
	//		30, 62
	//	};
	//
	//	ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	//}

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

		App->renderer->Blit(circle, x, y);

		c = c->next;
	}

	//// Rick Heads
	//c = ricks.getFirst();
	//while(c != NULL)
	//{
	//	int x, y;
	//	c->data->GetPosition(x, y);
	//	App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
	//	c = c->next;
	//}

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
	
	int ballx, bally;
	ballbod->GetPosition(ballx, bally);
	App->renderer->Blit(ball, ballx - 7, bally - 7);

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
			break;
		case ColliderType::WALL:
			ballbod->body->SetLinearVelocity(b2Vec2(0, 0));
			ballbod->body->SetTransform(b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0)), 0.0f);
			
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
	int x = SCREEN_WIDTH / 2 + 60;
	int y = SCREEN_HEIGHT / 1.5f + 70;

	b2BodyDef base;
	base.type = b2_staticBody;
	base.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* baseBody = App->physics->world->CreateBody(&base);

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(20), PIXEL_TO_METERS(10));

	b2FixtureDef fixture;
	fixture.shape = &shape;

	baseBody->CreateFixture(&fixture);

	PhysBody* yo = new PhysBody();
	yo->body = baseBody;
	baseBody->SetUserData(&yo);
	yo->ctype = ColliderType::BUMPER;

	yo->listener = this;
	baseBody->SetUserData(yo);


	x = SCREEN_WIDTH / 2 + 41;
	y = SCREEN_HEIGHT / 1.5f + 70;

	base.type = b2_dynamicBody;
	base.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add this static body to the World
	b2Body* palaBody = App->physics->world->CreateBody(&base);

	shape.SetAsBox(PIXEL_TO_METERS(20), PIXEL_TO_METERS(10));

	fixture.shape = &shape;

	palaBody->CreateFixture(&fixture);

	yo = new PhysBody();
	yo->body = palaBody;
	palaBody->SetUserData(&yo);
	yo->ctype = ColliderType::BUMPER;

	yo->listener = this;
	palaBody->SetUserData(yo);
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = baseBody;
	revoluteJointDef.bodyB = palaBody;
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.localAnchorA.Set(0, 0);
	revoluteJointDef.localAnchorB.Set(PIXEL_TO_METERS(20), 0);
}

void ModuleSceneIntro::ApplyVectorImpulse(PhysBody* bodyA, PhysBody* bodyB)
{
	int bumpx, ballx, bumpy, bally;

	bodyB->GetPosition(bumpx, bumpy);
	bodyA->GetPosition(ballx, bally);

	b2Vec2 forceDir = b2Vec2((ballx - bumpx), (bally - bumpy));

	bodyA->body->SetLinearVelocity(b2Vec2(0, 0));

	bodyA->body->ApplyLinearImpulse(0.03f * forceDir, bodyA->body->GetPosition(), true);

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

