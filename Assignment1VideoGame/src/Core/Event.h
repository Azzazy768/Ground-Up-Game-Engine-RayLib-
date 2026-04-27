#pragma once

enum class EventType { 
    WeaponFired,
    EnemyHit,   
    GameOver,
    GameWon
};

class Event {
public:
    virtual ~Event() = default;
    virtual EventType GetType() const = 0;
};