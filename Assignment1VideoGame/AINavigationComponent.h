#pragma once
#include "../src/Core/Component.h"
#include "NavigationGraph.h"
#include <vector>
#include <../src/Components//TransformComponent.h>
#include <../src/Core/GameObject.h>

class AINavigationComponent : public Component {
private:
	NavigationGraph* navGraph;
	std::vector<PathNode*> currentPath;
	int currentWaypointIndex = 0;
	float moveSpeed = 10.0f;

public:
	bool isActive = true;
	AINavigationComponent(GameObject* owner) : Component(owner) {}
	void SetNavigationGraph(NavigationGraph* graph) { navGraph = graph; }

	void CalculatePathTo(Vector3 targetWorldPos) { //Gets current world position, and calls the find path function to get the path needed to target
		if(navGraph != nullptr){
			TransformComponent* transform = owner->GetComponent<TransformComponent>();
			Vector3 currentWorldPos = transform->position;
			currentPath = navGraph->FindPath(currentWorldPos, targetWorldPos);
			currentWaypointIndex = 1;
		}
		else { return; }
	}
	void Update(float deltaTime) override {
		if(isActive == false){
			return;
		}
		if (currentPath.empty() || currentWaypointIndex >= currentPath.size()) {
			return;
		}

		TransformComponent* transform = owner->GetComponent<TransformComponent>();
		Vector3 currentPosition = transform->position;
		Vector3 targetPosition = currentPath[currentWaypointIndex]->position;

		targetPosition.y = currentPosition.y;
		Vector3 dir = Vector3Normalize(Vector3Subtract(targetPosition, currentPosition));
		Vector3 newPos = Vector3Add(currentPosition, Vector3Scale(dir, moveSpeed * deltaTime));
		transform->position = newPos;

		if(Vector3Distance(currentPosition,targetPosition) < 0.5){
			
			transform->position = targetPosition; //Snapping so that floating point drift is avoided
			currentWaypointIndex++; 
			if(currentWaypointIndex < currentPath.size()){ //Hard Turning as told by the assginment guide
				Vector3 nextNode = currentPath[currentWaypointIndex]->position;

				float dx = nextNode.x - currentPosition.x;
				float dz = nextNode.z - currentPosition.z;
		
				float angleRadians = atan2(dx, dz);
				transform->rotation = { 0.0f, angleRadians * RAD2DEG, 0.0f };

			}
				
		}
	
	};
	void Render() override {
		if (currentPath.empty()) return;

		else {
			for (int i = 0; i < currentPath.size() - 1; i++) {
				DrawLine3D(currentPath[i]->position, currentPath[i + 1]->position, RED);
			}
		}
	}

	static Component* CreateComponent(void* pData, GameObject* pOwner) {
		AINavigationComponent* navComp = new AINavigationComponent(pOwner);
		return navComp;
	}
};
