#include "Game.h"
#include "raylib.h"
#include "../Components/TransformComponent.h"
#include "../Game/GameEvents.h"
#include "../Components/TankRendererComponent.h"
#include "../Components/TankControllerComponent.h"
#include "../Components/MeshRendererComponent.h"
#include "../Components/ProjectileMovementComponent.h"
#include "../Components/AABBColliderComponent.h"
#include "../Core/GameObjectManager.h"
#include "../Core/StateMachine.h"
#include "../States/EnemyStates.h"
#include <iostream>

GameObject* playerTankPointer;
GameObject* enemyTankPointer;
GameObject* bulletPointer;

void OnWeaponFired(const Event& event) {
	const WeaponFiredEvent& weaponEvent = (const WeaponFiredEvent&)event;
	std::cout << "PEW!" << std::endl;
} 



Game::Game(int width, int height, std::string title) {
	InitWindow(width, height, title.c_str());
	Initialize();
}

Game::~Game(){
	Shutdown();
}

void Game::Initialize() {
	camera.position = { 0.0f,10.0f,10.0f };
	camera.target = { 0,0,0 };
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f; //The camera up and camera fov needed to be defined to display correctly (AI helped me debug this issue)

	GameObjectManager::Instance().RegisterComponentFactory("TransformComponent", TransformComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("TankControllerComponent", TankControllerComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("MeshRendererComponent", MeshRendererComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("AABBColliderComponent", AABBColliderComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("ProjectileMovementComponent", ProjectileMovementComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("StateMachineComponent", StateMachine::CreateComponent);

	//Adding states to state factory so that the json can read it and create states it needs for the fsm
	StateMachine::RegisterStateFactory("EnemyAliveState", EnemyAliveState::CreateState);
	StateMachine::RegisterStateFactory("EnemyDestroyedState", EnemyDestroyedState::CreateState);

	EventManager::Instance().AddListener(EventType::WeaponFired, OnWeaponFired); //Registers the function i made above for the sphere check into our event manager

	playerTankPointer = GameObjectManager::Instance().CreateGameObject("PlayerTank.json");
	enemyTankPointer = GameObjectManager::Instance().CreateGameObject("EnemyTank.json");
	bulletPointer = GameObjectManager::Instance().CreateGameObject("PlasmaBullet.json");


}


void Game::Run() {
	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();
		Update(deltaTime);
		Render();
	}
}

void Game::Update(float deltaTime) {
	GameObjectManager::Instance().Update(deltaTime);


	std::vector<GameObject*> allObjects = GameObjectManager::Instance().gameObjects;

	AABBColliderComponent* enemyCollider = enemyTankPointer->GetComponent<AABBColliderComponent>();

	for (GameObject* obj : allObjects) {
		if (obj->isActive && obj->GetComponent<ProjectileMovementComponent>() != nullptr) {

			AABBColliderComponent* bulletCollider = obj->GetComponent<AABBColliderComponent>();

			if (bulletCollider != nullptr && enemyCollider != nullptr && bulletCollider->Intersects(enemyCollider)) {
				obj->isActive = false;
				EnemyHitEvent hitEvent(enemyTankPointer);
				EventManager::Instance().TriggerEvent(hitEvent);
			}
		}
	}

}

void Game::Render() {
	BeginDrawing();
	ClearBackground(DARKPURPLE);
	BeginMode3D(camera);
	
	DrawGrid(20, 1.0f);



	//Rendering all objects
	GameObjectManager::Instance().Render();

	EndMode3D();
	EndDrawing();
}

void Game::Shutdown() {
	CloseWindow();
}
