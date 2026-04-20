#pragma once
#include "../Core/Component.h"
#include "raylib.h"
#include "../Components/TransformComponent.h"
#include "../Core/GameObject.h"
#include "AABBColliderComponent.h"
#include <cmath>
#include "json.hpp"
#include "rlgl.h"



class MeshRendererComponent :public Component {
public:
	Model model;
	Color color;
	GameObject* owner;

	MeshRendererComponent(GameObject* ownerPtr,Model theModel) {
		owner = ownerPtr; //Sets the owner of the component
		model = theModel;
	}

	~MeshRendererComponent() { //Unloads model before destorying object
		UnloadModel(model);
	}

	void Update(float deltaTime) override {}

	void Render() override {
		TransformComponent* transform = owner->GetComponent<TransformComponent>(); //Get the transform component from the object that owns this mesh renderer
		if (transform != nullptr) {
			model.transform = transform->getWorldPos(); //Get the world position of the verixes
			DrawModelWires(model, { 0, 0, 0 }, 1.0f, color);
		}

	}

	static Component* CreateComponent(void* pData, GameObject* pOwner) {
		nlohmann::json* j = static_cast<nlohmann::json*>(pData);

		std::string shape = (*j)["Shape"]; //Get the shape
		Mesh mesh;

		if (shape == "Cube") {
			float width = (*j)["Width"];
			float height = (*j)["Height"];
			float length = (*j)["Length"];

			mesh = GenMeshCube(width, height, length);
		}
		else if (shape == "Sphere") {
			float radius = (*j)["Radius"];
			mesh = GenMeshSphere(radius, 10, 10);
		}

		Model model = LoadModelFromMesh(mesh);
		MeshRendererComponent* renderer = new MeshRendererComponent(pOwner, model);

		if (j->contains("Color")) { //Checking and applying colors if there are any
			renderer->color = {
				(*j)["Color"][0],
				(*j)["Color"][1],
				(*j)["Color"][2],
				(*j)["Color"][3],
			};
			model.materials[0].maps[MATERIAL_MAP_ALBEDO].color = renderer->color;
		}

		BoundingBox localBounds = GetModelBoundingBox(model);

		Vector3 size = {  //Calculating size through local bounds distances
			localBounds.max.x - localBounds.min.x,
			localBounds.max.y - localBounds.min.y,
			localBounds.max.z - localBounds.min.z,
		};

		Vector3 offset = {
			localBounds.min.x + size.x / 2.0f,
			localBounds.min.y + size.y / 2.0f,
			localBounds.min.z + size.z / 2.0f
		};

		AABBColliderComponent* collider = pOwner->GetComponent<AABBColliderComponent>();
		if (collider != nullptr) {
			collider->Init(offset, size);
		}

		return renderer;
	}	
};