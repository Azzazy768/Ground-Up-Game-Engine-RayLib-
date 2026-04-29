#include "GameObjectManager.h"
#include <fstream>
#include <iostream>
#include "json.hpp"

GameObjectManager& GameObjectManager::Instance() {
	static GameObjectManager Instance;
	return Instance;
}


void GameObjectManager::RegisterComponentFactory(const std::string& typeName, ComponentCreator pFunc) {
	m_mComponentCreators[typeName] = pFunc;
}

GameObject* GameObjectManager::CreateGameObject(const std::string& filePath) {
	std::ifstream file(filePath);

	nlohmann::json j;
	file >> j; //Parsing the json file data.

	GameObject* newObject = new GameObject(); //Creating a blank/empty object
	newObject->SetName(j["Name"]);
	if (j.contains("Components") && j["Components"].is_array()) { //Checking if the json file has a components key, and if it has an array as a value
		for (auto& componentData : j["Components"]) {
			if (componentData.contains("Type")) {
				
				std::string typeName = componentData["Type"]; //Putting the value of the key "Type" inside a variable to be searched up in the m_mComponent map
				
				auto numInCompMap = m_mComponentCreators.find(typeName);
				
				if (numInCompMap != m_mComponentCreators.end()) { //if the result of find typeName does not equal map end (which means its inside the map, then..)
				
					Component* newCompontent = numInCompMap->second(&componentData, newObject); //Create a new component by calling the function that is stored as the value of the key in the map. (again, ->second is used when you want to get the value)
					newObject->AddComponent(newCompontent);
				}

			}
		}
	}
	gameObjects.push_back(newObject); //add the newly made object to the gameobjects list.
	return newObject;
}

void GameObjectManager::Update(float deltaTime) { //Loop and call update on every object
	for (size_t i = 0; i < gameObjects.size(); i++) {
        if (gameObjects[i] != nullptr && gameObjects[i]->isActive) {
            gameObjects[i]->Update(deltaTime);
        }
    }
}

void GameObjectManager::Render() {
	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i] != nullptr && gameObjects[i]->isActive) {
			gameObjects[i]->Render();
		}
	}
}

