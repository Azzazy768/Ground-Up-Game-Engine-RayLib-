#pragma once
#include <vector>
#include <string>

#include "Component.h"

class GameObject {
    std::vector<Component*> components;
    std::string name;

public:
    bool isActive;
    GameObject();
    ~GameObject();

    void Update(float deltaTime);
    void Render();

    void AddComponent(Component* component);

	std::string GetName() const { return name; }
	void SetName(const std::string& newName) { name = newName; }

    // Helper to get components (Optional but recommended)
    template <typename T>
    T* GetComponent() {
        for (auto c : components) {
            if (T* ptr = dynamic_cast<T*>(c)) return ptr;
        }
        return nullptr;
    }
};