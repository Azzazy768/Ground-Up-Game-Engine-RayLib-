#pragma once
#include <vector>
#include <map>
#include <functional>

#include "Event.h"

using EventCallback = std::function<void(const Event&)>;

class EventManager {
public:
    static EventManager& Instance() {
        static EventManager instance;
        return instance;
    }

    void AddListener(EventType type, EventCallback listener);
    void TriggerEvent(const Event& event);

private:
    std::map<EventType, std::vector<EventCallback>> listeners;
};