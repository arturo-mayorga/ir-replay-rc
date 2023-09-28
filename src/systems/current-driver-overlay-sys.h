#ifndef _current_driver_overlay_h
#define _current_driver_overlay_h

#include "../ecs.h"

#include <windows.h>

class CurrentDriverOverlaySystem : public ECS::EntitySystem
{
private:
    ECS::Entity *_canvasEnt;

public:
    virtual ~CurrentDriverOverlaySystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    void createWindow(ECS::World *world);

    void updateTables(ECS::World *world);
};

#endif