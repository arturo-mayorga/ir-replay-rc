#include "session-leaderboard-sys.h"

#include "../ecs-util.h"

#include "../components/session-comp.h"
#include "../components/car-comp.h"
#include "../components/session-comp.h"

#include <algorithm>

SessionLeaderBoardSystem::~SessionLeaderBoardSystem()
{
}

void SessionLeaderBoardSystem::configure(class ECS::World *world)
{
}

void SessionLeaderBoardSystem::unconfigure(class ECS::World *world)
{
}

void SessionLeaderBoardSystem::tick(class ECS::World *world, float deltaTime)
{
    auto leaderBoard = ECSUtil::getFirstCmp<SessionLeaderBoardComponentSP>(world);

    leaderBoard->carsByPosition.clear();

    int currentLap = 0;
    int totalLaps = ECSUtil::getFirstCmp<SessionComponentSP>(world)->lapCount;

    world->each<
        DynamicCarStateComponentSP,
        StaticCarStateComponentSP>(
        [&](
            ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> dState,
            ECS::ComponentHandle<StaticCarStateComponentSP> sState)
        {
            if (ent->get<StaticCarStateComponentSP>().get()->name.compare("Pace Car"))
            {
                if (dState.get()->currentLap > currentLap)
                {
                    currentLap = dState.get()->currentLap;
                }

                leaderBoard->carsByPosition.push_back(ent);
            }
        });

    leaderBoard->currentLap = currentLap;

    auto sessionResultComp = ECSUtil::getFirstCmp<SessionResultComponentSP>(world);

    std::sort(
        leaderBoard->carsByPosition.begin(), leaderBoard->carsByPosition.end(),
        [&](ECS::Entity *a, ECS::Entity *b)
        {
            auto aD = a->get<DynamicCarStateComponentSP>().get();
            auto bD = b->get<DynamicCarStateComponentSP>().get();

            // auto aS = a->get<StaticCarStateComponentSP>().get();
            // auto bS = b->get<StaticCarStateComponentSP>().get();

            // return sessionResultComp->carIndex2Position[aS->idx] < sessionResultComp->carIndex2Position[bS->idx];

            if (currentLap > totalLaps)
            {
                return sessionResultComp->carIndex2Position[aD->idx] < sessionResultComp->carIndex2Position[bD->idx];
            }

            return (aD->currentLap == bD->currentLap) ? aD->lapDistPct > bD->lapDistPct : aD->currentLap > bD->currentLap;
        });
}
