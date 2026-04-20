#pragma once
#include "raylib.h"
#include "../Core/Event.h"


class GameObject;

class WeaponFiredEvent : public Event {
public:
	Vector3 origin;
	float power;

	WeaponFiredEvent(Vector3 shotOrigin, float shotPower) {
		origin = shotOrigin;
		power = shotPower;
	}

	EventType GetType() const{ //needed const here so that i could pass fireEvent as a event in TankControllerComponent.h
		return EventType::WeaponFired;
	}
};

class EnemyHitEvent : public Event {
public:
	GameObject* enemy;

	EnemyHitEvent(GameObject* hitEnemy) { //getting the enemy we just hit
		enemy = hitEnemy;
	}

	EventType GetType() const override {
		return EventType::EnemyHit;
	}
};