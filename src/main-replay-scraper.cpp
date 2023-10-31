#include "ecs.h"
#include <iostream>

#include <Windows.h>
#include <sysinfoapi.h>

#include "systems/irtelemetry-sys.h"
#include "systems/irtelemetry-fake-sys.h"
#include "systems/overtake-detector-system.h"
#include "systems/incident-detector-system.h"
#include "systems/closest-battle-director-sys.h"
#include "systems/broadcast-car-info-collector-sys.h"
#include "systems/broadcast-summary-sys.h"
#include "systems/tv-point-director-sys.h"
#include "systems/head-of-direction-sys.h"
#include "systems/scraper-system.h"

#include "components/car-comp.h"
#include "components/cam-ctrl-comp.h"

#include "components/app-state-comp.h"
#include "components/session-comp.h"

#include <ctime>

int main()
{
    ECS::World *world = ECS::World::createWorld();

    ScraperSystem *scraperSystem;

    world->registerSystem(new IrTelemetrySystem());
    world->registerSystem(scraperSystem = new ScraperSystem());

    ECS::Entity *ent = world->create();
    ent->assign<CameraActualsComponentSP>(new CameraActualsComponent());
    ent->assign<SessionComponentSP>(new SessionComponent());
    ent->assign<SessionResultComponentSP>(new SessionResultComponent());

    auto tPrev = GetTickCount();

    while (!scraperSystem->isFinished())
    {
        auto t = GetTickCount();
        world->tick((float)(t - tPrev));
        Sleep(10);
        tPrev = t;
    }

    world->destroyWorld();

    return 0;
}