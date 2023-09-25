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
    DONE
};

void playSoundForTime(int time)
{
    std::stringstream sout;
    sout << "open \"script_" << time << ".wav\" type mpegvideo alias mp3";
    std::cout << "playing: " << sout.str() << std::endl;
    mciSendString((LPCSTR)(sout.str().c_str()), NULL, 0, NULL);
    mciSendString((LPCSTR) "play mp3 wait", NULL, 0, NULL);
    mciSendString("close mp3", NULL, 0, NULL);
    std::cout << "audio finished" << std::endl;
}

ReplayDirectorSystem::~ReplayDirectorSystem() {}
void ReplayDirectorSystem::configure(class ECS::World *world) {}
void ReplayDirectorSystem::unconfigure(class ECS::World *world) {}

void ReplayDirectorSystem::tick(class ECS::World *world, float deltaTime)
{
    // std::cout << "ReplayDirectorSystem::tick" << std::endl;

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

                std::cout << "driver map " << cState->idx << " " << cState->uid << std::endl;

                state = SEEKING_JUMP;
            });

        scriptItemIndex = 0;
        // state = SEEKING;
        break;
    }
    case SEEKING:
    {
        if (camScript->items[scriptItemIndex]->time > 0)
        {
            state = SEEKING_JUMP;
        }
        else
        {
            state = SEEKING_WAIT;
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

                playSoundForTime(time);
            }

            state = VIEWING_WAIT;
        }
        break;
    }
    case VIEWING_WAIT:
    {
        if (cameraActualsComponent->replayFrameNum >= camScript->items[scriptItemIndex]->time + 3 * 60)
        {
            state = SEEKING;
            scriptItemIndex++;

            if (scriptItemIndex > camScript->items.size() - 1)
            {
                state = DONE;
            }
        }

        break;
    }
    case SEEKING_JUMP:
    {
        int time = camScript->items[scriptItemIndex]->time - 3 * 60;
        int idx = uid2driverIdx[camScript->items[scriptItemIndex]->driverId];

        std::cout << "time " << time << " idx " << idx << " uid " << camScript->items[scriptItemIndex]->driverId << std::endl;

        world->emit<OnFrameNumChangeRequest>(OnFrameNumChangeRequest(time));
        world->emit<OnCameraChangeRequest>(OnCameraChangeRequest(idx));

        playSoundForTime(camScript->items[scriptItemIndex]->time);

        state = VIEWING_JUMP;
        break;
    }
    case VIEWING_JUMP:
    {
        if (cameraActualsComponent->replayFrameNum >= camScript->items[scriptItemIndex]->time + 2 * 60)
        {
            state = SEEKING;
            scriptItemIndex++;

            if (scriptItemIndex > camScript->items.size() - 1)
            {
                state = DONE;
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
