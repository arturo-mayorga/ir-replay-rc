#include "ecs.h"
#include "ecs-util.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/irtelemetry-fake-sys.h"
#include "systems/window-rendering-sys.h"
#include "systems/session-leaderboard-sys.h"
#include "systems/timing-tower-sys.h"
#include "systems/current-driver-overlay-sys.h"
#include "systems/replay-skip-graphic-sys.h"

#include "components/car-comp.h"
#include "components/session-comp.h"
#include "components/cam-ctrl-comp.h"
#include "components/timing-tower-config-comp.h"

#include <ctime>
#include <tchar.h>

#include <locale>
#include <codecvt>
#include <string>

void loadConfig(int argc, char *argv[], ECS::World *world)
{
    if (argc > 1)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

        ECS::Entity *timingTowerConfig = world->create();
        TimingTowerConfigComponent cfg;
        cfg.league = converter.from_bytes(argv[1]);
        timingTowerConfig->assign<TimingTowerConfigComponent>(cfg);
    }
}

int main(int argc, char *argv[])
{
    ECS::World *world = ECS::World::createWorld();

    loadConfig(argc, argv, world);

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