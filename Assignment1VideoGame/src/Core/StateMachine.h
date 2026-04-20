#pragma once
#include <map>
#include "State.h"
#include "Component.h"
#include <string>

typedef State* (*StateCreationFunction)(); //Similiar to component factory

class StateMachine : public Component {


private: 
	std::map<int, State*> registeredStates;
	State* currentState = nullptr;
public:

	static std::map<std::string, StateCreationFunction> stateFactories;

	StateMachine(GameObject* owner) : Component(owner) { currentState = nullptr; }
	~StateMachine() override = default;


	static void RegisterStateFactory(std::string name, StateCreationFunction createFunction); //Since we dont have a single instance of FSM like GameObjectManager, we are using a static method

	int initialState = -1;

	void RegisterState(int stateId, State* stateInstance);
	void GoToState(int stateId);
	
	void Update(float deltaTime) override;
	void Render() override {} //Nothing to add here cause FSM's dont do any rendering, they only handle logic.

	static Component* CreateComponent(void* pData, GameObject* pOwner);
};