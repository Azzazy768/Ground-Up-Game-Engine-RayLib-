//I decided to give the player a finite state machine to make it easier to turn off and on the player
#pragma once
#include "../src/Core/State.h"
#include "../src/Core/GameObject.h"
#include "../src/Components/MeshRendererComponent.h"
#include "../src/Components/AABBColliderComponent.h"
#include "../AINavigationComponent.h"
#include "raylib.h"
#include <iostream>
#include "../src/Core/EventManager.h"
#include "../src/Core/StateMachine.h"
#include "../src/Components/TankControllerComponent.h"

enum PlayerStates {
	PLAYERALIVE = 0,
	PLAYERDEAD = 1,
	PLAYERWINNER = 2
};

class PlayerAliveState : public State {
public:
	static State* CreateState() { return new PlayerAliveState(); }

	void Enter() override {

		EventManager::Instance().AddListener(EventType::EnemyHit, [this](const Event& e) {
			const EnemyHitEvent& hitEvent = static_cast<const EnemyHitEvent&>(e);
			if (hitEvent.enemy == this->owner) {
				EventManager::Instance().TriggerEvent(GameOverEvent{});
			}
			});


		EventManager::Instance().AddListener(EventType::GameWon, [this](const Event& event) {
			std::cout << "Player Won!" << std::endl;
			owner->GetComponent<StateMachine>()->GoToState(PLAYERWINNER);
		});

		EventManager::Instance().AddListener(EventType::GameOver, [this](const Event& event) {
			std::cout << "Player Dead!" << std::endl;
			owner->GetComponent<StateMachine>()->GoToState(PLAYERDEAD);
			});
	}

	void Update(float deltaTime) override {
	}
	void Exit() override {}

};

class PlayerDeadState : public State {
public:
	static State* CreateState() { return new PlayerDeadState(); }

	void Enter() override {
		AABBColliderComponent* collider = owner->GetComponent<AABBColliderComponent>();
		collider->isActive = false;

		MeshRendererComponent* renderer = owner->GetComponent<MeshRendererComponent>();
		renderer->color = DARKGRAY; 

		TankControllerComponent* controller = owner->GetComponent<TankControllerComponent>();
		controller->isActive = false; //Turns off player movement

	}

	void Update(float deltaTime) override {
	}
	void Exit() override {}
};


class PlayerWinnerState : public State {
public:
	static State* CreateState() { return new PlayerDeadState(); }

	void Enter() override {
		TankControllerComponent* controller = owner->GetComponent<TankControllerComponent>();
		controller->isActive = false; //Turns off player movement
	}

	void Update(float deltaTime) override {
	}
	void Exit() override {}
};