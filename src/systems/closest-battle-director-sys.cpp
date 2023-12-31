#include "closest-battle-director-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"

#include <iostream>
#include <algorithm>

ClosestBattleDirectorSystem::~ClosestBattleDirectorSystem()
{
}

void ClosestBattleDirectorSystem::configure(class ECS::World *world)
{
}

void ClosestBattleDirectorSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

#define MIN_PCT_DELTA 0.005

void ClosestBattleDirectorSystem::tick(class ECS::World *world, float deltaTime)
{
    std::vector<DynamicCarStateComponentSP> states;

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            if (cState->isInPits == 0 && cState->deltaLapDistPct * 100000 > MIN_PCT_DELTA)
            {
                // ignore cars if they are in the pits or moving too slow
                states.push_back(cState);
            }
        });

    // sort the states by track position
    std::sort(states.begin(), states.end(), [](DynamicCarStateComponentSP &a, DynamicCarStateComponentSP &b)
              { return a->lapDistPct > b->lapDistPct; });

    float minInterval = 2;
    int minIntervalCarIdx = -1;

    int prevIdx = -1;
    float prevLapDistPct = -1;

    for (auto c : states)
    {
        if (prevIdx == -1)
        {
            prevIdx = c->idx;
            prevLapDistPct = c->lapDistPct;
            continue;
        }

        if (prevLapDistPct < 0 || c->lapDistPct < 0)
        {
            continue;
        }

        auto currentInterval = prevLapDistPct - c->lapDistPct;

        if (currentInterval < minInterval)
        {
            minInterval = currentInterval;
            minIntervalCarIdx = c->idx;
        }

        prevIdx = c->idx;
        prevLapDistPct = c->lapDistPct;
    }

    world->each<CameraDirectionSubTargetsComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraDirectionSubTargetsComponentSP> cStateH)
        {
            CameraDirectionSubTargetsComponentSP cState = cStateH.get();
            if (minIntervalCarIdx > 0) // make sure we don't try to select the pace car
            {
                cState->closestBattleCarIdx = minIntervalCarIdx;
            }
        });
}
