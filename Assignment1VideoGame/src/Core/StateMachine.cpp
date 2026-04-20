#include "StateMachine.h"
#include "json.hpp"


std::map<std::string, StateCreationFunction> StateMachine::stateFactories; //Static state factory

void StateMachine::RegisterState(int stateId, State* stateInstance) {
	if (stateInstance != nullptr) {
		stateInstance->Init(this, owner); //Passing this statemachine and the owner into the state, so that the state can add it into its own variables
		registeredStates[stateId] = stateInstance; //Putting this state into the state machine map according to its ID 
	}
}

void StateMachine::GoToState(int stateId) {
	if (currentState != nullptr) { //Exit the current state if its not null by using the "Exit()" method each state has.
		currentState->Exit(); 
	} 
	if (registeredStates.find(stateId) != registeredStates.end()){ //Basiaclly saying "if this ID is found to be not equal to the end (lastState+1). then do this"
		currentState = registeredStates[stateId];
		currentState->Enter();
	}
}

void StateMachine::RegisterStateFactory(std::string name, StateCreationFunction createFunction) {
	stateFactories[name] = createFunction;
}

void StateMachine::Update(float deltaTime){
	if(currentState != nullptr){
		currentState->Update(deltaTime); //Call the update for the current state.
	}
}


Component* StateMachine::CreateComponent(void* pData, GameObject* pOwner) {
	nlohmann::json* j = static_cast<nlohmann::json*>(pData);
	StateMachine* stateMachine = new StateMachine(pOwner);
	if (j->contains("States")) {
		for (auto& stateFromJson : (*j)["States"]){
			int stateID = stateFromJson["ID"];
			std::string stateName = stateFromJson["Name"];
			if (stateFactories.find(stateName) != stateFactories.end()) {
				State* newState = stateFactories[stateName](); //Creating the state using its name and searching it up in the factory list
				stateMachine->RegisterState(stateID,newState); //Registering it in the FSM
			}
		}
	}
	if (j->contains("InitialState")) {
		int startState = (*j)["InitialState"];
		stateMachine->GoToState(startState);
	}
	return stateMachine;
}