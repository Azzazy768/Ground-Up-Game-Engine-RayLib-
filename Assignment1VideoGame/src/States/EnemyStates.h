#pragma once
#include "../Core/State.h"
#include "../Core/GameObject.h"
#include "../Components/MeshRendererComponent.h"
#include "../Components/AABBColliderComponent.h"
#include "raylib.h"
#include <iostream>

enum EnemyStates {
	ALIVE = 0,
	DEAD = 1,
};

class EnemyAliveState :public State {
public:
	static State* CreateState() { return new EnemyAliveState(); } //Creating static method for state factory

	void Enter() {
		EventManager::Instance().AddListener(EventType::EnemyHit, [this](const Event& event) { //When a enemy tank is made and is entered in alive, it registers into the event system a hitEvent. I  was unsure where to implement the registration of the event. So i decided here is best.
			const EnemyHitEvent& hitEvent = (const EnemyHitEvent&)event;
			if (hitEvent.enemy == this->owner) {
				std::cout << "Enemy Hit!";
				owner->GetComponent<StateMachine>()->GoToState(DEAD); 
			}
			});
	}
	void Update(float deltaTime) {}
	void Exit() {}
};

class EnemyDestroyedState : public State {
public:

	static State* CreateState() { return new EnemyDestroyedState(); } //Creating static method for state factory

	void Enter() {
		AABBColliderComponent* collider = owner->GetComponent<AABBColliderComponent>();
		collider->isActive = false;


		MeshRendererComponent* renderer = owner->GetComponent<MeshRendererComponent>();
		renderer->color = DARKGRAY;
		renderer->model.materials[0].maps[MATERIAL_MAP_ALBEDO].color = DARKGRAY;
	}

	void Update(float deltaTime) {}

	void Exit() {}
};