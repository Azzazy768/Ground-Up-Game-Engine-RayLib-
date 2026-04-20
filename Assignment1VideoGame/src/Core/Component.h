#pragma once


class GameObject;

class Component {
public:

	GameObject* owner; //I had to add gameobject owner pointers to accomodate the changes in other components.

	Component() {
		owner = nullptr; //Default constructer in case a component is made without an object 
	}

	Component(GameObject* ownerPointer) {
		owner = ownerPointer;
	}

	virtual ~Component() = default;

	virtual void Update(float deltaTime){}
	virtual void Render() {}
};