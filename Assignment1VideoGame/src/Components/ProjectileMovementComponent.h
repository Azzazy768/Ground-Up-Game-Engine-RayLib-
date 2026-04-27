#pragma once
#include "../Core/Component.h"
#include "TransformComponent.h"
#include "../Core/GameObject.h"
#include "json.hpp"
#include <cmath>

class ProjectileMovementComponent :public Component {
public:
	float velocity = 0.0f;
	float currentLife = 0.0f;
	float lifetime = 0.0f;
	GameObject* tankOwner = nullptr; //Who shot the projectile

	ProjectileMovementComponent(GameObject* owner) : Component(owner) {}
	~ProjectileMovementComponent() override = default;

	void Update(float deltaTime) override {
		currentLife += deltaTime; //Increment current life of bullet using deltaTiem

		if (currentLife >= lifetime) { //If bigger than dedicated lifetime, turn the object off.
			owner->isActive = false;
			return;
		}

		TransformComponent* transform = owner->GetComponent<TransformComponent>(); //get the objects transform component
		if (transform != nullptr) {
			
			Vector3 direction = { //get the direction 
				sinf(transform->rotation.y),0.0f,cosf(transform->rotation.y)
			};

			Vector3 moveAmount = { //get the movement amount (using deltaTime for consistency as usual)
				direction.x * velocity * deltaTime,
				0,
				direction.z * velocity * deltaTime
			};

			transform->Translate(moveAmount); 
		}
	}
	static Component* CreateComponent(void* pData, GameObject* pOwner) {
		nlohmann::json* j = static_cast<nlohmann::json*>(pData);
		ProjectileMovementComponent* projectile = new ProjectileMovementComponent(pOwner);
		if (j->contains("Velocity")) {
			projectile->velocity = (*j)["Velocity"];
		}
		if (j->contains("Lifetime")) {
			projectile->lifetime = (*j)["Lifetime"];
		}
		return projectile;
	}
};