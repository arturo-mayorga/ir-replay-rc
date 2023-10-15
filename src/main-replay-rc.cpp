#include "ecs.h"
#include "ecs-util.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/replay-director-system.h"

#include "components/car-comp.h"
#include "components/session-comp.h"
#include "components/cam-ctrl-comp.h"

#include <ctime>
#include <tchar.h>

CameraScriptComponentSP loadScript(ECS::World *world)
{
    std::cout << "Loading Script..." << std::endl;
    int scriptItemCount = 0;
    std::cin >> scriptItemCount;

    ECS::Entity *camScriptEnt = world->create();
    auto camScript = camScriptEnt->assign<CameraScriptComponentSP>(new CameraScriptComponent()).get();
    for (int i = 0; i < scriptItemCount; ++i)
    {
        int time = 0;
        int driverId = 0;

        std::cin >> time;
        std::cin >> driverId;

        camScript.get()->items.push_back(CameraScriptItemSP(new CameraScriptItem(time, driverId)));
    }

    std::cout << scriptItemCount << std::endl;

    std::cout << "Starting Loop..." << std::endl;

    return camScript;
}

int main()
{
    ECS::World *world = ECS::World::createWorld();

    // todo: move the creation of this entity into the irtelemetry system
    ECS::Entity *ent = world->create();
    ent->assign<CameraActualsComponentSP>(new CameraActualsComponent());
    ent->assign<SessionComponentSP>(new SessionComponent());
    ent->assign<SessionResultComponentSP>(new SessionResultComponent());

    ent->assign<CameraDirectionSubTargetsComponentSP>(new CameraDirectionSubTargetsComponent());

    world->registerSystem(new IrTelemetrySystem());
    world->registerSystem(new ReplayDirectorSystem());

    auto tPrev = GetTickCount();

    auto script = loadScript(world);

    while (script->done == 0)
    {
        auto t = GetTickCount();
        world->tick((float)(t - tPrev));
        Sleep(100);
        tPrev = t;
    }

    world->destroyWorld();

    return 0;
}