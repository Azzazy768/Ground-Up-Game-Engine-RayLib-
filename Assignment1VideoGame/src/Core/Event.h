#pragma once

enum class EventType { WeaponFired, EntityMoved, Collision,EnemyHit };

class Event {
public:
    virtual ~Event() = default;
    virtual EventType GetType() const = 0;
};