#ifndef _timing_tower_h
#define _timing_tower_h

#include "../ecs.h"

#include <windows.h>

class TimingTowerSystem : public ECS::EntitySystem
{
private:
    ECS::Entity *_canvasEnt;

public:
    virtual ~TimingTowerSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    void createWindow(ECS::World *world);

    void updateTables(ECS::World *world);
};

#endif