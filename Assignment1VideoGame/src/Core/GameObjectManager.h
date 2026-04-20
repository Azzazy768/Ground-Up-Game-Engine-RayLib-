#pragma once
#include <map>
#include <string>
#include <vector>
#include "GameObject.h"
#include "json.hpp"


typedef Component* (*ComponentCreator)(void* pDataNode, GameObject* pOwner);

class GameObjectManager {
private:
	std::map<std::string, ComponentCreator> m_mComponentCreators;

public:
	std::vector<GameObject*> gameObjects;
	static GameObjectManager& Instance();


	//Registering a component with its static creator
	void RegisterComponentFactory(const std::string& typeName, ComponentCreator pFunc);
	

	//Parses file, then constructs object
	GameObject* CreateGameObject(const std::string& filePath);

	void Update(float deltaTime);
	void Render();

};