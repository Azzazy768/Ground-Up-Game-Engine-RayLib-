#pragma once
#include "../Core/Component.h"
#include "raylib.h"
#include "../Components/TransformComponent.h"
#include "../Core/EventManager.h"
#include "../Game/GameEvents.h"
#include <cmath>
#include "json.hpp"
#include "../Core/GameObjectManager.h"
#include<iostream>
#include "../Components/ProjectileMovementComponent.h"

class TankControllerComponent : public Component {
	TransformComponent* tankTransformation;
	float tankSpeed = 3.f;
	float rotationSpeed = 2.f;

public:

	TankControllerComponent(GameObject* owner, TransformComponent* transform) : Component(owner) {
		tankTransformation = transform;
	}


	void Update(float deltaTime) {


		Vector3 dir = {
			sinf(tankTransformation->rotation.y),0.0f,cosf(tankTransformation->rotation.y ) //Getting the direction the player is looking at.
		};

		Vector3 TempMovement = { 0,0,0 }; 

		if (tankTransformation != nullptr) {
			if (IsKeyDown(KEY_W)) {
				TempMovement = { dir.x * tankSpeed * deltaTime,0,dir.z*tankSpeed*deltaTime};
				tankTransformation->Translate(TempMovement);
			}
			if (IsKeyDown(KEY_S)) {
				TempMovement = { -dir.x * tankSpeed * deltaTime, 0, -dir.z * tankSpeed * deltaTime };
				tankTransformation->Translate(TempMovement);
			}
			if (IsKeyDown(KEY_A)) {
				tankTransformation->Rotate({ 0,rotationSpeed*deltaTime,0 });
			}
			if (IsKeyDown(KEY_D)) {
				tankTransformation->Rotate({ 0,-rotationSpeed*deltaTime,0 });
			}
			if (IsKeyPressed(KEY_SPACE)) { 

				GameObject* bullet = GameObjectManager::Instance().CreateGameObject("PlasmaBullet.json");
				if (bullet != nullptr) {
					TransformComponent* bulletTransform = bullet->GetComponent<TransformComponent>();
					if (bulletTransform != nullptr) {
						bulletTransform->position = tankTransformation->position; //Setting the bullet's position to be the tanks.
						bulletTransform->position.y += 0.6f; //Raising the bullet a small bit so it comes out of the turret, not the tank body
						bulletTransform->rotation = tankTransformation->rotation; //Setting its rotation to be the same as the tanks
						bullet->GetComponent<ProjectileMovementComponent>()->tankOwner = owner;
					}
					
				}
			



				WeaponFiredEvent fireEvent(tankTransformation->position, 10.f);
				EventManager::Instance().TriggerEvent(fireEvent);
			}
		}
	}

	static Component* CreateComponent(void* pData, GameObject* pOwner) {
		nlohmann::json* j = static_cast<nlohmann::json*>(pData);
		TankControllerComponent* controller = new TankControllerComponent(pOwner, pOwner->GetComponent<TransformComponent>());
		if (j->contains("MoveSpeed")) {
			controller->tankSpeed = (*j)["MoveSpeed"];
		}
		if (j->contains("TurnSpeed")) {
			controller->rotationSpeed = (*j)["TurnSpeed"];
		}
		return controller;
	}
};