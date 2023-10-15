#ifndef _replay_skip_graphic_h
#define _replay_skip_graphic_h

#include "../ecs.h"

#include <windows.h>

class ReplaySkipGraphicSystem : public ECS::EntitySystem
{
private:
    ECS::Entity *_canvasEnt;

public:
    virtual ~ReplaySkipGraphicSystem();

    virtual void configure(class ECS::World *world) override;
    virtual void unconfigure(class ECS::World *world) override;
    virtual void tick(class ECS::World *world, float deltaTime) override;

    void createWindow(ECS::World *world);
    void updateTables(ECS::World *world, float deltaTime);
};

#endif