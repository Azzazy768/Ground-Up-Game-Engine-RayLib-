#include "EventManager.h"
#include "raylib.h"


void EventManager::AddListener(EventType type, EventCallback listener) {
	listeners[type].push_back(listener);
}

void EventManager::TriggerEvent(const Event& event) {
	EventType type = event.GetType();

	auto eventRow = listeners.find(type); //auto finds out the type for me. cleaner implementation (found it online)
	if (eventRow != listeners.end()) {
		for (const auto& listener : eventRow->second) { //eventRow->second means that: second is the value in the keyvalue pairs, the vectors of functions. so the code here is saying, run the functions found in this vector.
			listener(event);
		}
	}
}