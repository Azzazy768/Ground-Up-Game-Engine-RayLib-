#include "GameObject.h"

GameObject::GameObject() {
	isActive = true;
}

GameObject::~GameObject() {  //Making a deconstructer for the objects so that i can avoid memory leaks after being done with any object 
	for (Component* component : components) {
		delete component;
	}
	components.clear();
}

void GameObject::Update(float deltaTime){
	if (!isActive) {
		return;
	}
	for (Component* component : components) {
		component->Update(deltaTime); //Because we are passing a pointer and not the object itself, i need to use -> instead of using component.Render().
	}
}

void GameObject::Render() {
	if (!isActive) {
		return;
	}
	for (Component* component : components) {
		component->Render(); //Same thing like i did in the update, just for rendering this time.
	}
}

void GameObject::AddComponent(Component* component) {
	if (component != nullptr) {
		components.push_back(component);
	}
}