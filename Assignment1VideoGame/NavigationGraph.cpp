#include "NavigationGraph.h"
#include <algorithm>
#include <cmath>


NavigationGraph::NavigationGraph(int gridWidth, int gridHeight, float cellSize) {
	width = gridWidth;
	height = gridHeight;
	cellSize = cellSize;

	grid.resize(width); //resizing array to be the amount of indexes as needed

	for (int x = 0; x < width; ++x) {
		grid[x].resize(height);
		for (int y = 0; y < height; ++y) {
			Vector3 worldPos = { x * cellSize, 0.0f, y * cellSize }; //convert from world pos to grid pos
			grid[x][y] = new PathNode(worldPos, x, y);
		}
	}
}

std::vector<PathNode*> NavigationGraph::FindPath(Vector3 startPos, Vector3 targetPos) {
	
	for (int x = 0; x < width; x++) { //Reseting all nodes
		for (int y = 0; y < height; y++) {
			grid[x][y]->Reset();
		}
	}

	PathNode* startNode = GetNodeFromWorldPosition(startPos);
	PathNode* targetNode = GetNodeFromWorldPosition(targetPos);

	if (startNode == nullptr || targetNode == nullptr) {
		return std::vector<PathNode*>(); //Returning an empty path if start or the target is null
	}

	std::vector<PathNode*> openList;
	openList.push_back(startNode);
	startNode->inOpenList = true;



	while (openList.empty() != true) {

		PathNode* lowestNode = openList[0];
		int lowestIndex = 0;

		for (int i = 1; i < openList.size(); i++) {
			if (lowestNode->fCost > openList[i]->fCost) {
				lowestNode = openList[i];
				lowestIndex = i;
			}

		}

		openList.erase(openList.begin() + lowestIndex);
		lowestNode->inOpenList = false;
		lowestNode->inClosedList = true;

		if (lowestNode == targetNode) {
			break;
		}

		std::vector<PathNode*> neighbors = GetNeighbors(lowestNode);
		for(PathNode* neighbor : neighbors){
			if (neighbor->inClosedList || !neighbor->IsPassable()) {
				break;
			}
			else {
				float moveCostToNeighbor = lowestNode->gCost + Vector3Distance(lowestNode->position, neighbor->position);
				if(!neighbor->inOpenList || moveCostToNeighbor < neighbor->gCost){
					
					neighbor->parent = lowestNode;
					neighbor->gCost = moveCostToNeighbor;
					neighbor->hCost = CalculateHeuristic(neighbor, targetNode);
					neighbor->fCost = neighbor->gCost + neighbor->hCost;

					if(neighbor->inOpenList == false){
						openList.push_back(neighbor);
						neighbor->inOpenList = true;
					}

				}

			}
		}

		std::vector<PathNode*> path;
		if (targetNode->parent != nullptr || targetNode == startNode) {
			PathNode* currentNode = targetNode;
			while(currentNode != nullptr) { //Following the parents backwards and putting them inside the path list
				path.push_back(currentNode);
				currentNode = currentNode->parent;
			}

			std::reverse(path.begin(), path.end()); //Reverseing the path so we go from start -> finish instead of finish -> start
		}
		return path;

	}
}

float NavigationGraph::CalculateHeuristic(PathNode* a, PathNode* b) {
	float Xdistance = std::abs(a->gridX - b->gridX);
	float Ydistance = std::abs(a->gridY - b->gridY);
	return Xdistance + Ydistance; 
}

std::vector<PathNode*> NavigationGraph::GetNeighbors(PathNode* node) {
	std::vector<PathNode*> neighbors;
	int checkXPos, checkYPos;

	//Checks right node
	checkXPos = node->gridX + 1; checkYPos = node->gridY;
	if (checkXPos >= 0 && checkXPos < width && checkYPos >= 0 && checkYPos < height)
		neighbors.push_back(grid[checkXPos][checkYPos]);

	//Checks left node
	checkXPos = node->gridX - 1; checkYPos = node->gridY;
	if (checkXPos >= 0 && checkXPos < width && checkYPos >= 0 && checkYPos < height)
		neighbors.push_back(grid[checkXPos][checkYPos]);

	//Checks up node
	checkXPos = node->gridX; checkYPos = node->gridY + 1;
	if (checkXPos >= 0 && checkXPos < width && checkYPos >= 0 && checkYPos < height)
		neighbors.push_back(grid[checkXPos][checkYPos]);

	//Checks down node
	checkXPos = node->gridX; checkYPos = node->gridY - 1;
	if (checkXPos >= 0 && checkXPos < width && checkYPos >= 0 && checkYPos < height)
		neighbors.push_back(grid[checkXPos][checkYPos]);

	return neighbors;
}	

void NavigationGraph::SetStaticObstacle(int x, int y, bool isObstacle) { 
	if (x >= 0 && x < width && y >= 0 && y < height) { //Check that the x and y are inside the grid
		grid[x][y]->isStaticObstacle = isObstacle; //set the position in the grid to be an obstacle depending on the boolean that was entered
	}
}

PathNode* NavigationGraph::GetNodeFromWorldPosition(Vector3 worldPos) {
	int gridX = static_cast<int>(std::round(worldPos.x / cellSize)); //Get the grid x and y by diving the world position by the cell size  and round it manually 
	int gridY = static_cast<int>(std::round(worldPos.z / cellSize));


	if (gridX < 0) { //Make it so that if the grid x or y values goes outside of the play area, return it to a proper value to stop crashes
		gridX = 0;
	} 
	if (gridX >= width) {
		gridX = width - 1;
	}
	if (gridY < 0) {
		gridY = 0;
	}
	if (gridY >= height) {
		gridY = height - 1;
	}

	return grid[gridX][gridY];
}