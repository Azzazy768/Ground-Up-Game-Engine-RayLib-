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
#include "../AINavigationComponent.h"
#include <iostream>
#include "../LevelLoader.h"

GameObject* playerTankPointer;
GameObject* enemyTankPointer;
GameObject* bulletPointer;
NavigationGraph* levelGraph = nullptr;

void OnWeaponFired(const Event& event) {
	const WeaponFiredEvent& weaponEvent = (const WeaponFiredEvent&)event;
	std::cout << "PEW!" << std::endl;
} 

void CheckBulletCollisions() { //Loops through objects, finds bullets, loops through objects again and sees if theres collision between bullet and tank or wall. and does appropiate action.
	std::vector<GameObject*> allObjects = GameObjectManager::Instance().gameObjects;
	for (GameObject* currentObject : allObjects) {
		if (currentObject->GetName() == "PlasmaBullet") {
			if (currentObject->isActive == true) {
				AABBColliderComponent* bulletCollider = currentObject->GetComponent<AABBColliderComponent>();
				if(bulletCollider != nullptr){
					for (GameObject* targetObject : allObjects) {
						if ((targetObject->GetName() == "EnemyTank" || targetObject->GetName() == "PlayerTank") && targetObject != currentObject->GetComponent<ProjectileMovementComponent>()->tankOwner) {
							if (bulletCollider->Intersects(targetObject->GetComponent<AABBColliderComponent>())) {
								currentObject->isActive = false; //Turn off the bullet
								EventManager::Instance().TriggerEvent(EnemyHitEvent{ targetObject }); //Trigger the enemy hit event, which will be listened to by the enemy alive state, which will then transition the enemy to its dead state.
							}
						}
						else if (targetObject->GetName() == "Wall") {
							if(bulletCollider->Intersects(targetObject->GetComponent<AABBColliderComponent>())) {
								currentObject->isActive = false; //Turn off the bullet if it hits a wall as well
							}
						}
					}
				}
			}
		}
	}
}




Game::Game(int width, int height, std::string title) {
	InitWindow(width, height, title.c_str());
	Initialize();
}

Game::~Game(){
	Shutdown();
}

void Game::Initialize() {
	camera.position = { 32.0f, 70.0f, 80.0f };
	camera.target = { 32.0f, 0.0f, 32.0f }; 
	camera.projection = CAMERA_PERSPECTIVE;
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f; //The camera up and camera fov needed to be defined to display correctly (AI helped me debug this issue)

	GameObjectManager::Instance().RegisterComponentFactory("TransformComponent", TransformComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("TankControllerComponent", TankControllerComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("MeshRendererComponent", MeshRendererComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("AABBColliderComponent", AABBColliderComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("ProjectileMovementComponent", ProjectileMovementComponent::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("StateMachineComponent", StateMachine::CreateComponent);
	GameObjectManager::Instance().RegisterComponentFactory("AINavigationComponent", AINavigationComponent::CreateComponent);

	//Adding states to state factory so that the json can read it and create states it needs for the fsm
	StateMachine::RegisterStateFactory("EnemyChaseState", EnemyChaseState::CreateState);
	StateMachine::RegisterStateFactory("EnemyAttackState", EnemyAttackState::CreateState);
	StateMachine::RegisterStateFactory("EnemyDestroyedState", EnemyDestroyedState::CreateState);
	
	EventManager::Instance().AddListener(EventType::WeaponFired, OnWeaponFired); //Registers the function i made above for the sphere check into our event manager

	//Loading level
	levelGraph = new NavigationGraph(16, 16, 4.0f);
	LevelLoader loader = LevelLoader();
	loader.LoadLevel("Level.json", levelGraph);

	

}


void Game::Run() {
	while (!WindowShouldClose()) {
		float deltaTime = GetFrameTime();
		Update(deltaTime);
		Render();
	}
}

void Game::Update(float deltaTime) {
	std::vector<GameObject*> allObjects = GameObjectManager::Instance().gameObjects;

	//Saves old positions before movement
	for (GameObject* obj : allObjects) {
		TransformComponent* transform = obj->GetComponent<TransformComponent>();
		if (transform != nullptr) {
			transform->previousPosition = transform->position;
		}
	}

	//Update all objects
	GameObjectManager::Instance().Update(deltaTime);

	//Update colliders (NEEDED TO ADD THIS TO STOP GETTING STUCK ON WALLS)
	for (GameObject* obj : allObjects) {
		AABBColliderComponent* collider = obj->GetComponent<AABBColliderComponent>();
		if (collider != nullptr && collider->isActive) {
			collider->Update(0.0f);
		}
	}


	//Check if any object has collided with wall.
	for (GameObject* obj : allObjects) {
		if (obj->GetName() == "PlayerTank" || obj->GetName() == "EnemyTank") {

			AABBColliderComponent* objCollider = obj->GetComponent<AABBColliderComponent>();
			TransformComponent* objTransform = obj->GetComponent<TransformComponent>();

			if (objCollider == nullptr || !objCollider->isActive) continue;

			for (GameObject* wall : allObjects) {
				if (wall->GetName() == "Wall") {
					AABBColliderComponent* wallCollider = wall->GetComponent<AABBColliderComponent>();

					// If the tank walked into a wall, push it back to its old position
					if (wallCollider != nullptr && objCollider->Intersects(wallCollider)) {
						objTransform->position = objTransform->previousPosition;
					
					}
				}
			}
		}
	}

	CheckBulletCollisions();



	 
}

void Game::Render() {
	BeginDrawing();
	ClearBackground(DARKPURPLE);
	BeginMode3D(camera);


	for (int i = 0; i <= 16; i++) {
		DrawLine3D({ i * 4.0f, 0.0f, 0.0f }, { i * 4.0f, 0.0f, 64.0f }, LIGHTGRAY); // Vertical
		DrawLine3D({ 0.0f, 0.0f, i * 4.0f }, { 64.0f, 0.0f, i * 4.0f }, LIGHTGRAY); // Horizontal
	}


	//Rendering all objects
	GameObjectManager::Instance().Render();

	EndMode3D();
	EndDrawing();
}

void Game::Shutdown() {
	CloseWindow();
}

/*std::vector<GameObject*> allObjects = GameObjectManager::Instance().gameObjects;

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
	}*/