#include "irtelemetry-fake-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/session-comp.h"

#include "../ecs-util.h"

#include <iostream>

#include <signal.h>
#include <time.h>

#include <vector>
#include <map>
#include <set>
#include <memory>

#include <Windows.h>

std::vector<std::map<int, int>> IrTelemetryFakeSystem_getSessionResultsMap(int numCars)
{

    std::vector<std::map<int, int>> ret;

    std::map<int, int> sessionResults;

    for (int i = 0; i < numCars; ++i)
    {
        sessionResults[i] = numCars - i + 1;
    }

    ret.push_back(sessionResults);

    return ret;
}

std::map<int, int> IrTelemetryFakeSystem_getSessionLapCountMap()
{
    std::map<int, int> ret;

    ret[0] = 100;

    return ret;
}

std::map<int, std::string> IrTelemetryFakeSystem_getSessionNameMap()
{
    std::map<int, std::string> ret;

    ret[0] = "Test Session";

    return ret;
}

std::map<int, StaticCarStateComponentSP> IrTelemetryFakeSystem_getStaticCarStates(int numCars)
{
    std::map<int, StaticCarStateComponentSP> ret;

    for (int i = 0; i < numCars; ++i)
    {
        // make up some values for the car
        StaticCarStateComponentSP cState(new StaticCarStateComponent());
        cState->idx = i;
        cState->uid = 1000 + i;
        cState->number = numCars + i;
        cState->name = "Car " + std::to_string(i);
        ret[i] = cState;
    }

    return ret;
}

int IrTelemetryFakeSystem_waitForData()
{
    static int callCount = 0;
    Sleep(16);
    ++callCount;
    return callCount > 50;
}

std::vector<float> IrTelemetryFakeSystem_getCarSpeeds(int numCars)
{
    std::vector<float> ret;

    float targetSecondsPerLap = 10;
    float framesPerSecond = 30;
    float percentPerFrame = 1 / (targetSecondsPerLap * framesPerSecond);

    for (int i = 0; i < numCars; ++i)
    {
        // reduce the percent per frame by a random amount between 0 and 3%
        float percentPerFrameRand = percentPerFrame * (1 - (rand() % 3) / 100.0);
        ret.push_back(percentPerFrameRand);
    }

    return ret;
}

IrTelemetryFakeSystem::~IrTelemetryFakeSystem()
{
}

void IrTelemetryFakeSystem::configure(class ECS::World *world)
{
    world->subscribe<OnCameraChangeRequest>(this);
    world->subscribe<OnFrameNumChangeRequest>(this);
    _carSpeeds = IrTelemetryFakeSystem_getCarSpeeds(_numCars);
}

void IrTelemetryFakeSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void IrTelemetryFakeSystem::receive(ECS::World *world, const OnFrameNumChangeRequest &event)
{
    _currentFrameNum = event.frameNum;
}

void IrTelemetryFakeSystem::receive(ECS::World *world, const OnCameraChangeRequest &event)
{
    _currentCarIndex = event.targetCarIdx;
}

void IrTelemetryFakeSystem::tick(class ECS::World *world, float deltaTime)
{
    static int first = 1;
    static int lastCam = -1;
    static float tSinceCamChange = 0;
    static float tSinceIrData = 0;
    static std::map<int, std::string> sessionNameMap;
    static std::map<int, int> sessionLapsMap;
    static std::vector<std::map<int, int>> sessionResultsMap;
    static int maxSessionIdx = -1;
    static int tickCount = 0;

    tSinceCamChange += deltaTime;
    tSinceIrData += deltaTime;

    ++tickCount;
    if (tickCount > 60 * 30)
    {
        // update the car speeds every 30 secondsy
        _carSpeeds = IrTelemetryFakeSystem_getCarSpeeds(_numCars);
        tickCount = 0;
    }

    // wait up to 16 ms for start of session or new data
    if (IrTelemetryFakeSystem_waitForData())
    {
        // and grab the data
        if (first)
        {
            first = 0;

            auto cStates = IrTelemetryFakeSystem_getStaticCarStates(_numCars);
            sessionNameMap = IrTelemetryFakeSystem_getSessionNameMap();
            sessionLapsMap = IrTelemetryFakeSystem_getSessionLapCountMap();
            sessionResultsMap = IrTelemetryFakeSystem_getSessionResultsMap(_numCars);

            for (auto it = sessionNameMap.begin(); it != sessionNameMap.end(); ++it)
            {
                if (-1 * it->first > maxSessionIdx)
                {
                    maxSessionIdx = -1 * it->first;
                }
            }

            std::set<int> existingCarIdxs;

            std::vector<ECS::Entity *> entsToDestroy;

            world->each<StaticCarStateComponentSP>(
                [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
                {
                    StaticCarStateComponentSP cState = cStateH.get();

                    if (cStates.count(cState->idx))
                    {
                        existingCarIdxs.insert(cState->idx);
                        cState->idx = cStates[cState->idx]->idx;
                        cState->number = cStates[cState->idx]->number;
                    }
                    else
                    {
                        entsToDestroy.push_back(ent);
                    }
                });

            for (auto pair : cStates)
            {
                if (existingCarIdxs.count(pair.first))
                {
                }
                else
                {

                    ECS::Entity *ent = world->create();
                    auto staticCarState = ent->assign<StaticCarStateComponentSP>(new StaticCarStateComponent());
                    auto dynamicCarState = ent->assign<DynamicCarStateComponentSP>(new DynamicCarStateComponent());
                    auto broadcastCarState = ent->assign<BroadcastCarInfoComponentSP>(new BroadcastCarInfoComponent());
                    auto broadcastCarSummary = ent->assign<BroadcastCarSummaryComponentSP>(new BroadcastCarSummaryComponent());

                    staticCarState.get()->idx = pair.second->idx;
                    staticCarState.get()->name = pair.second->name;
                    staticCarState.get()->uid = pair.second->uid;
                    staticCarState.get()->number = pair.second->number;

                    dynamicCarState.get()->idx = pair.second->idx;
                    dynamicCarState.get()->currentLap = 0;
                    dynamicCarState.get()->lapDistPct = 0;
                    broadcastCarState.get()->idx = pair.second->idx;
                    broadcastCarSummary.get()->idx = pair.second->idx;
                }
            }

            CameraActualsComponentSP cameraActualsCmp = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
            cameraActualsCmp->replayFrameNumEnd -= _totalFrames;
        }

        world->each<DynamicCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
            {
                DynamicCarStateComponentSP cState = cStateH.get();

                int i = cState->idx;

                if (i >= 0)
                {
                    float oldLapPct = cState->lapDistPct;
                    cState->lapDistPct += _carSpeeds[i];
                    cState->officialPos = i;
                    cState->isInPits = false;

                    if (cState->lapDistPct > 1)
                    {
                        cState->lapDistPct = 0;
                        cState->currentLap += 1;
                    }

                    float deltaLapDistPct = cState->lapDistPct - oldLapPct;
                    if (deltaLapDistPct < 0)
                    {
                        deltaLapDistPct += 1;
                    }

                    deltaLapDistPct /= tSinceIrData;
                    cState->deltaLapDistPct = deltaLapDistPct;
                }
                else
                {
                }
            });

        SessionComponentSP sessionComp = ECSUtil::getFirstCmp<SessionComponentSP>(world);

        sessionComp->num = 0;
        sessionComp->name = sessionNameMap[sessionComp->num];
        sessionComp->lapCount = sessionLapsMap[sessionComp->num];

        SessionResultComponentSP sessionResultComp = ECSUtil::getFirstCmp<SessionResultComponentSP>(world);

        sessionResultComp->carIndex2Position = sessionResultsMap[sessionComp->num];

        tSinceIrData = 0;

        CameraActualsComponentSP cameraActualsCmp = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
        cameraActualsCmp->timeSinceLastChange = tSinceCamChange;
        cameraActualsCmp->replayFrameNum = ++_currentFrameNum;
        cameraActualsCmp->replayFrameNumEnd = _totalFrames - _currentFrameNum;
        cameraActualsCmp->currentCarIdx = _currentCarIndex;
        if (cameraActualsCmp->currentCarIdx != lastCam)
        {
            tSinceCamChange = 0;
            lastCam = cameraActualsCmp->currentCarIdx;
        }
    }
}
