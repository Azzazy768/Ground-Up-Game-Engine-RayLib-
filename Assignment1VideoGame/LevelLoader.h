#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "NavigationGraph.h"
#include "../src/Core/GameObject.h"
#include "../src/Core/GameObjectManager.h"
#include "../src/Components/TransformComponent.h"
#include "../AINavigationComponent.h"


class LevelLoader {
public:
	void LoadLevel(const std::string& filename, NavigationGraph* navGraph) {

        std::ifstream file(filename); //Opening the file using c++ file reading library
        if (!file.is_open()) {
            std::cout << "Could not load level" << filename << std::endl;
            return;
        }

		nlohmann::json levelData = nlohmann::json::parse(file);

        int widthOfGrid = levelData["GridWidth"];
		int heightOfGrid = levelData["GridHeight"];
		float cellSize = levelData["CellSize"];
		auto grid = levelData["Grid"];
		for (int currentRow = 0; currentRow < heightOfGrid; currentRow++) { //Iterating through every row
			for (int currentColumn = 0; currentColumn < widthOfGrid; currentColumn++) { //Iterating through every column in the row
				int currentCell = grid[currentRow][currentColumn];
				Vector3 cellWorldPos = { currentColumn * cellSize, 0.0f, currentRow * cellSize }; //Calculating the cells world position
				
				if (currentCell == 1) {
					GameObject* wall = GameObjectManager::Instance().CreateGameObject("Wall.json"); 
					wall->GetComponent<TransformComponent>()->position = { cellWorldPos.x, 2.0f, cellWorldPos.z }; //Changing the position of the wall to be the one we got previously from calculation
					if (navGraph != nullptr) {
						navGraph->SetStaticObstacle(currentColumn, currentRow, true); //Setting this cell to be a static obstacle in the nav graph
					}
				}
				else if (currentCell == 2) {
					GameObject* enemy = GameObjectManager::Instance().CreateGameObject("EnemyTank.json");
					enemy->GetComponent<TransformComponent>()->position = { cellWorldPos.x, 0.5, cellWorldPos.z };
					enemy->GetComponent<AINavigationComponent>()->SetNavigationGraph(navGraph); //giving the ai component the nav graph
				}
				else if (currentCell == 3) {
					GameObject* player = GameObjectManager::Instance().CreateGameObject("PlayerTank.json");
					player->GetComponent<TransformComponent>()->position = { cellWorldPos.x, 0.5, cellWorldPos.z };
				}
			}
		
		}
       
	}
};