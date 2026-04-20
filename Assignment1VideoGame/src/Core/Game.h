#pragma once
#include "raylib.h"
#include <string>
#include "GameObject.h"

class Game {
public:
    Game(int width, int height, std::string title);
    ~Game();
    void Run();

private:
    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();

    Camera3D camera;
    bool isRunning;
    GameObject* playerTankPointer;
    GameObject* enemyTankPointer;
};