#include "ecs.h"
#include "ecs-util.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/window-rendering-sys.h"
#include "systems/session-leaderboard-sys.h"
#include "systems/timing-tower-sys.h"
#include "systems/current-driver-overlay-sys.h"
#include "systems/replay-skip-graphic-sys.h"

#include "components/car-comp.h"
#include "components/session-comp.h"
#include "components/cam-ctrl-comp.h"

#include <ctime>
#include <tchar.h>

int main()
{
    ECS::World *world = ECS::World::createWorld();

    ECS::Entity *ent = world->create();
    ent->assign<CameraActualsComponentSP>(new CameraActualsComponent());
    ent->assign<SessionComponentSP>(new SessionComponent());
    ent->assign<SessionResultComponentSP>(new SessionResultComponent());

    ent->assign<SessionLeaderBoardComponentSP>(new SessionLeaderBoardComponent());

    world->registerSystem(new IrTelemetrySystem());
    world->registerSystem(new SessionLeaderBoardSystem());
    world->registerSystem(new TimingTowerSystem());
    // world->registerSystem(new ReplaySkipGraphicSystem());
    world->registerSystem(new CurrentDriverOverlaySystem());
    world->registerSystem(new WindowRenderingSystem());

    auto tPrev = GetTickCount();

    while (true)
    {
        auto t = GetTickCount();
        world->tick((float)(t - tPrev));
        Sleep(1);
        tPrev = t;
    }

    world->destroyWorld();

    return 0;
}