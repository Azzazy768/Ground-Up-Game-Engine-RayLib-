#pragma once
#include "../Core/State.h"
#include "../Core/GameObject.h"
#include "../Components/MeshRendererComponent.h"
#include "../Components/AABBColliderComponent.h"
#include "../AINavigationComponent.h"
#include "raylib.h"
#include <iostream>

enum EnemyStates {
	CHASE = 0,
	ATTACK = 1,
	DEAD = 2,
};


GameObject* GetClosestAliveTank(Vector3 myPos, GameObject* currentGameObj) {
	GameObject* closestTank = nullptr;
	float closestDistance = -1;


	for (GameObject* targetObj : GameObjectManager::Instance().gameObjects) {
		if (targetObj != currentGameObj && (targetObj->GetName() == "EnemyTank" || targetObj->GetName() == "PlayerTank") && targetObj->GetComponent<AABBColliderComponent>()->isActive == true) { //Gets the type and filters out walls and bullets
			TransformComponent* targetObjTransform = targetObj->GetComponent<TransformComponent>(); //Gets the world position of the target object
			if (targetObjTransform != nullptr) {
				float distance = Vector3Distance(myPos, targetObjTransform->position); //gets the difference between the currentObject and the target obj its calculating
				if (closestDistance == -1) { //Checks if distance is initiliazed at first (as distance cannot be negative)
					closestDistance = distance;
					closestTank = targetObj;
				}
				else {
					if (distance < closestDistance) { //if min distance is bigger than the distance calculated, replace min distance.
						closestDistance = distance;
						closestTank = targetObj;
					}
				}
			}
		}
	}

	return closestTank;
}


class EnemyChaseState : public State {
	float pathTimer = 1.0f;
	float pathUpdateInterval = 1.0f;
	bool isHitRegistered = false; 

public:
	static State* CreateState() { return new EnemyChaseState(); }

	void Enter() override {
		
		if (!isHitRegistered) { //Registering the hit event here
			EventManager::Instance().AddListener(EventType::EnemyHit, [this](const Event& event) {
				const EnemyHitEvent& hitEvent = (const EnemyHitEvent&)event;
				if (hitEvent.enemy == this->owner) {
					std::cout << "Tank Hit!" << std::endl;
					owner->GetComponent<StateMachine>()->GoToState(DEAD);
				}
				});
			isHitRegistered = true;
		}
	}

	void Update(float deltaTime) override {
		pathTimer += deltaTime;
		GameObject* closestTank = GetClosestAliveTank(owner->GetComponent<TransformComponent>()->position, owner);

		if (closestTank != nullptr) {
			TransformComponent* enemyTransform = closestTank->GetComponent<TransformComponent>();
			float distanceToTarget = Vector3Distance(owner->GetComponent<TransformComponent>()->position, enemyTransform->position);

			
			if (distanceToTarget <= 10.0f) {
				owner->GetComponent<StateMachine>()->GoToState(ATTACK);
				return;
			}

			if (pathTimer >= pathUpdateInterval) {
				owner->GetComponent<AINavigationComponent>()->CalculatePathTo(enemyTransform->position);
				pathTimer = 0.0f;
			}
		}
	}
	void Exit() override {}
};



class EnemyAttackState : public State {
	float shootTimer = 1.5f;
	float shootInterval = 1.5f;

public:
	static State* CreateState() { return new EnemyAttackState(); }

	void Enter() override {
	}

	void Update(float deltaTime) override {
		shootTimer += deltaTime;
		GameObject* closestTank = GetClosestAliveTank(owner->GetComponent<TransformComponent>()->position, owner);

		if (closestTank == nullptr) { //if there are no tanks nearby, go to chase state
			owner->GetComponent<StateMachine>()->GoToState(CHASE);
			return;
		}

		TransformComponent* myTransform = owner->GetComponent<TransformComponent>();
		TransformComponent* targetTransform = closestTank->GetComponent<TransformComponent>();
		float distanceToTarget = Vector3Distance(myTransform->position, targetTransform->position);

		
		if (distanceToTarget > 12.0f) { //if distance from target more than 12, go to chase state
			owner->GetComponent<StateMachine>()->GoToState(CHASE);
			return;
		}

		
		float distanceX = targetTransform->position.x - myTransform->position.x;
		float distanceZ = targetTransform->position.z - myTransform->position.z;
		float angleRadians = atan2(distanceX, distanceZ);
		myTransform->rotation = { 0.0f, angleRadians, 0.0f }; 

		if (shootTimer >= shootInterval) {
			GameObject* bullet = GameObjectManager::Instance().CreateGameObject("PlasmaBullet.json");
			if (bullet != nullptr) {
				TransformComponent* bulletTransform = bullet->GetComponent<TransformComponent>();
				Vector3 bulletPosition = {
					sinf(myTransform->rotation.y) * 1.5f,
					0.5f,
					cosf(myTransform->rotation.y) * 1.5f
				};
				bulletTransform->position = Vector3Add(myTransform->position, bulletPosition);
				bulletTransform->rotation = { 0.0f, angleRadians, 0.0f };
				bullet->GetComponent<ProjectileMovementComponent>()->tankOwner = this->owner;
			}
			shootTimer = 0.0f;
		}
	}
	void Exit() override {}
};


class EnemyDestroyedState : public State {
public:

	static State* CreateState() { return new EnemyDestroyedState(); } //Creating static method for state factory

	void Enter() {
		AABBColliderComponent* collider = owner->GetComponent<AABBColliderComponent>();
		collider->isActive = false;

		AINavigationComponent* navComponent = owner->GetComponent<AINavigationComponent>();
		navComponent->isActive = false;

		MeshRendererComponent* renderer = owner->GetComponent<MeshRendererComponent>();
		renderer->color = DARKGRAY;
		renderer->model.materials[0].maps[MATERIAL_MAP_ALBEDO].color = DARKGRAY;
	}

	void Update(float deltaTime) {}

	void Exit() {}
};