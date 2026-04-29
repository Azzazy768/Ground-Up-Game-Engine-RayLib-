#pragma once
#include "../Core/Component.h"
#include "raylib.h"
#include "../Core/GameObject.h"
#include "../Components/TransformComponent.h"
#include "raymath.h"

class AABBColliderComponent :public Component {
private:
	Vector3 offset;
	Vector3 size;

	Vector3 minBounds;
	Vector3 maxBounds;

public:
	bool isActive = true; //Added this flag so i can turn off the AABBCollider as the assignment had asked me to do 


	AABBColliderComponent(GameObject* ownerPtr) : Component(ownerPtr) {}
	
	~AABBColliderComponent() override = default;

	void Init(Vector3 localOffset, Vector3 boxSize) {
		offset = localOffset;
		size = boxSize;
	}
	void Update(float deltaTime) override {
		TransformComponent* transform = owner->GetComponent<TransformComponent>(); //Made a temp transform component rather than having to use the long pointer call of (owner->GetComponent<Transform>()->GetPosition();). So now its just transform->getPosition() 

		if (!isActive) {
			return;
		}

		if (transform != nullptr) { 
			Matrix matrixWorld = transform->getWorldPos();

			Vector3 worldPos = { //Gets the last column (the world position).
				matrixWorld.m12,
				matrixWorld.m13,
				matrixWorld.m14,
			};

			Vector3 trueCenter = {
				worldPos.x + offset.x,
				worldPos.y + offset.y,
				worldPos.z + offset.z
			};

			Vector3 extents = {
				size.x / 2.0f,
				size.y / 2.0f,
				size.z / 2.0f
			};

			minBounds = trueCenter - extents;
			maxBounds = trueCenter + extents;

		}
	}
	void Render() override {
		if (!isActive) {
			return;
		}


		
	}

	bool Intersects(const AABBColliderComponent* other) const {
		if (!isActive|| !other->isActive) { return false; } //Checks if the current enity or the other entity is turned off. If yes, return no intersection. if both are on, continue as normal.

		return(
			this->maxBounds.x > other->minBounds.x &&
			this->minBounds.x < other->maxBounds.x &&

			this->maxBounds.y > other->minBounds.y &&
			this->minBounds.y < other->maxBounds.y &&

			this->maxBounds.z > other->minBounds.z &&
			this->minBounds.z < other->maxBounds.z );
	}

	static Component* CreateComponent(void* pData, GameObject* pOwner) {
		return new AABBColliderComponent(pOwner);
	}

};