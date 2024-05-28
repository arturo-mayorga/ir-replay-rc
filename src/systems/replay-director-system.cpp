#include <Windows.h>

#include "replay-director-system.h"
#include "../ecs-util.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/car-comp.h"

#include <iostream>
#include <sstream>
#include <map>

enum ReplayState
{
    LOADING,
    SEEKING,
    SEEKING_WAIT,
    SEEKING_JUMP,
    VIEWING_WAIT,
    VIEWING_JUMP,
    SPIN,
    DONE
};

ReplayDirectorSystem::~ReplayDirectorSystem() {}
void ReplayDirectorSystem::configure(class ECS::World *world) {}
void ReplayDirectorSystem::unconfigure(class ECS::World *world) {}

void ReplayDirectorSystem::tick(class ECS::World *world, float deltaTime)
{
    static int scriptItemIndex = -1;
    static ReplayState state = LOADING;
    static std::map<int, int> uid2driverIdx;

    CameraActualsComponentSP cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    CameraScriptComponentSP camScript = ECSUtil::getFirstCmp<CameraScriptComponentSP>(world);

    switch (state)
    {

    case LOADING:
    {
        world->each<StaticCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
            {
                StaticCarStateComponentSP cState = cStateH.get();
                uid2driverIdx[cState->uid] = cState->idx;

                state = SEEKING_JUMP;
            });

        scriptItemIndex = 0;

        if (camScript->items.size() == 0)
        {
            std::cout << "Entering SPIN mode for testing" << std::endl;
            state = SPIN;
            // std::cout << "new state: " << state << std::endl;
        }

        break;
    }
    case SPIN:
    {
        break;
    }
    case SEEKING:
    {
        if (camScript->items[scriptItemIndex]->time > 0)
        {
            state = SEEKING_JUMP;
            // std::cout << "new state: " << state << std::endl;
        }
        else
        {
            state = SEEKING_WAIT;
            // std::cout << "new state: " << state << std::endl;
        }

        break;
    }
    case SEEKING_WAIT:
    {
        int time = -1 * camScript->items[scriptItemIndex]->time;
        int idx = uid2driverIdx[camScript->items[scriptItemIndex]->driverId];

        if (cameraActualsComponent->replayFrameNum >= time)
        {
            if (time + 6 * 60 > cameraActualsComponent->replayFrameNum)
            {
                std::cout << "time " << time << " idx " << idx << " uid " << camScript->items[scriptItemIndex]->driverId << std::endl;
                world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(idx));
            }

            state = VIEWING_WAIT;
            // std::cout << "new state: " << state << std::endl;

            if (scriptItemIndex >= camScript->items.size() - 1)
            {
                state = DONE;
                // std::cout << "new state: " << state << std::endl;
            }
        }
        break;
    }
    case VIEWING_WAIT:
    {
        if (cameraActualsComponent->replayFrameNum >= camScript->items[scriptItemIndex]->time + 3 * 60)
        {
            state = SEEKING;
            // std::cout << "new state: " << state << std::endl;
            scriptItemIndex++;

            if (scriptItemIndex > camScript->items.size() - 1)
            {
                state = DONE;
                // std::cout << "new state: " << state << std::endl;
            }
        }

        break;
    }
    case SEEKING_JUMP:
    {
        int time = camScript->items[scriptItemIndex]->time;
        int idx = uid2driverIdx[camScript->items[scriptItemIndex]->driverId];

        std::cout << "time " << time << " idx " << idx << " uid " << camScript->items[scriptItemIndex]->driverId << std::endl;

        world->emit<OnFrameNumChangeRequest>(OnFrameNumChangeRequest(time));
        world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(idx));

        state = VIEWING_JUMP;
        // std::cout << "new state: " << state << std::endl;
        break;
    }
    case VIEWING_JUMP:
    {

        int extraTime = 2 * 60;
        if (scriptItemIndex >= camScript->items.size() - 1)
        {
            extraTime = 0;
        }

        if (cameraActualsComponent->replayFrameNum >= camScript->items[scriptItemIndex]->time + extraTime)
        {
            state = SEEKING;
            // std::cout << "new state: " << state << std::endl;
            scriptItemIndex++;

            if (scriptItemIndex > camScript->items.size() - 1)
            {
                state = DONE;
                // std::cout << "new state: " << state << std::endl;
            }
        }
        break;
    }
    case DONE:
    {
        camScript->done = 1;
        break;
    }
    }
}
