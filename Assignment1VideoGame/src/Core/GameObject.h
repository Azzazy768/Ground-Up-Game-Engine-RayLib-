#pragma once
#include <vector>
#include <string>

#include "Component.h"

class GameObject {
    std::vector<Component*> components;

public:
    bool isActive;
    GameObject();
    ~GameObject();

    void Update(float deltaTime);
    void Render();

    void AddComponent(Component* component);

    // Helper to get components (Optional but recommended)
    template <typename T>
    T* GetComponent() {
        for (auto c : components) {
            if (T* ptr = dynamic_cast<T*>(c)) return ptr;
        }
        return nullptr;
    }
};