#include "irtelemetry-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/session-comp.h"

#include "../ecs-util.h"

#include <Windows.h>

#include <iostream>

#define MIN_WIN_VER 0x0501

#ifndef WINVER
#define WINVER MIN_WIN_VER
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT MIN_WIN_VER
#endif

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <signal.h>
#include <time.h>

#include "../irsdk/irsdk_defines.h"
#include "../irsdk/irsdk_client.h"
#include "../irsdk/yaml_parser.h"

#include <vector>
#include <map>
#include <set>
#include <memory>

// for timeBeginPeriod
#pragma comment(lib, "Winmm")

irsdkCVar g_camCarIdx("CamCarIdx");
irsdkCVar g_camGroup("CamGroupNumber");

irsdkCVar g_sessionNum("SessionNum");

irsdkCVar g_CarIdxLapDistPct("CarIdxLapDistPct");
irsdkCVar g_carIdxClassPosition("CarIdxClassPosition");
irsdkCVar g_carIdxLap("CarIdxLap");
irsdkCVar g_isCarInPits("CarIdxTrackSurface");

irsdkCVar g_replayFrameNum("ReplayFrameNum");
irsdkCVar g_replayFrameNumEnd("ReplayFrameNumEnd");

void monitorConnectionStatus()
{
    // keep track of connection status
    bool isConnected = irsdkClient::instance().isConnected();
    static bool wasConnected = !isConnected;
    if (wasConnected != isConnected)
    {
        if (isConnected)
        {
            // printf("Connected to iRacing\n");
        }
        else
        {
            //   printf("Lost connection to iRacing\n");
        }
        wasConnected = isConnected;
    }
}

int getYamlInt(const char *yaml, const char *key)
{
    char valstr[512];
    int valstrlen = 512;

    const char *tVal = NULL;
    int tValLen = 0;

    if (parseYaml(yaml, key, &tVal, &tValLen))
    {
        int len = tValLen;
        if (len > 512)
            len = 512;

        // copy what we can, even if buffer too small
        memcpy(valstr, tVal, len);
        valstr[len] = '\0'; // original string has no null termination...

        return atoi(valstr);
    }
    else
    {
        return -1;
    }
}

int getSubsessionId(const char *yaml)
{
    return getYamlInt(yaml, "WeekendInfo:SubSessionID:");
}

std::vector<std::map<int, int>> getSessionResultsMap(const char *yaml)
{
    char str[512];
    int sessionI = 0;

    std::vector<std::map<int, int>> ret;

    do
    {
        int positionI = 0;

        std::map<int, int> sessionResults;

        sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionLaps:", sessionI);
        int sessionCheck = getYamlInt(yaml, str);
        do
        {
            if (positionI == 0)
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}ResultsPositions:Position:", sessionI);
            }
            else
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}ResultsPositions:Position:{%d}Position:", sessionI, positionI);
            }
            int p = getYamlInt(yaml, str);

            if (positionI == 999999)
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}ResultsPositions:Position:CarIdx:", sessionI);
            }
            else
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}ResultsPositions:Position:{%d}CarIdx:", sessionI, positionI + 1);
            }
            int cIdx = getYamlInt(yaml, str);

            ++positionI;
            if (p < 0)
            {
                positionI = -1;
            }
            else
            {
                sessionResults[cIdx] = p;
            }
        } while (positionI != -1);

        ++sessionI;

        if (sessionCheck < 0)
        {
            sessionI = -1;
        }
        else
        {
            ret.push_back(sessionResults);
        }
    } while (sessionI != -1);

    return ret;
}

std::map<int, int> getSessionLapCountMap(const char *yaml)
{
    std::map<int, int> ret;
    std::map<int, int> inter;

    char valstr[512];
    int valstrlen = 512;
    char str[512];

    const char *tVal = NULL;
    int tValLen = 0;

    int i = 0;
    int maxSessionIdx = -1;

    do
    {
        sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionLaps:", i);
        if (parseYaml(yaml, str, &tVal, &tValLen))
        {
            int len = tValLen;
            if (len > 512)
                len = 512;

            // copy what we can, even if buffer too small
            memcpy(valstr, tVal, len);
            valstr[len] = '\0'; // original string has no null termination...

            int sessionLaps = atoi(valstr);
            int sessionNum = -1;

            if (i == 0)
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:");
            }
            else
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionNum:", i - 1);
            }
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                sessionNum = atoi(valstr);
            }

            inter[sessionNum] = sessionLaps;

            maxSessionIdx = i;

            ++i;
        }
        else
        {
            i = -1;
        }
    } while (i != -1);

    for (int i = 0; i <= maxSessionIdx; ++i)
    {
        ret[i - maxSessionIdx] = inter[i];
    }

    return ret;
}

std::map<int, std::string> getSessionNameMap(const char *yaml)
{
    std::map<int, std::string> ret;
    std::map<int, std::string> inter;

    char valstr[512];
    int valstrlen = 512;
    char str[512];

    const char *tVal = NULL;
    int tValLen = 0;

    int i = 0;
    int maxSessionIdx = -1;

    do
    {
        sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionName:", i);
        if (parseYaml(yaml, str, &tVal, &tValLen))
        {
            int len = tValLen;
            if (len > 512)
                len = 512;

            // copy what we can, even if buffer too small
            memcpy(valstr, tVal, len);
            valstr[len] = '\0'; // original string has no null termination...

            std::string name(valstr);
            int sessionNum = -1;

            if (i == 0)
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:");
            }
            else
            {
                sprintf_s(str, 512, "SessionInfo:Sessions:SessionNum:{%d}SessionNum:", i - 1);
            }
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                sessionNum = atoi(valstr);
            }

            inter[sessionNum] = name;

            maxSessionIdx = i;

            ++i;
        }
        else
        {
            i = -1;
        }
    } while (i != -1);

    for (int i = 0; i <= maxSessionIdx; ++i)
    {
        ret[i - maxSessionIdx] = inter[i];
    }

    return ret;
}

bool invalidChar(char c)
{
    return !(c >= 0 && c < 128);
}

std::map<int, StaticCarStateComponentSP> getStaticCarStates(const char *yaml)
{
    // TODO: figure out a cheaper way to do this
    std::map<int, StaticCarStateComponentSP> ret;
    char valstr[512];
    int valstrlen = 512;
    char str[512];

    const char *tVal = NULL;
    int tValLen = 0;

    int i = 0;

    do
    {
        sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}UserName:", i);
        if (parseYaml(yaml, str, &tVal, &tValLen))
        {
            int len = tValLen;
            if (len > 512)
                len = 512;

            // copy what we can, even if buffer too small
            memcpy(valstr, tVal, len);
            valstr[len] = '\0'; // original string has no null termination...

            StaticCarStateComponentSP cState(new StaticCarStateComponent());
            cState->name = valstr;

            cState->name.erase(remove_if(cState->name.begin(), cState->name.end(), invalidChar), cState->name.end());

            if (i == 0)
            {
                sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:");
            }
            else
            {
                sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarIdx:", i - 1);
            }
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->idx = atoi(valstr);
            }

            sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}UserID:", i);
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->uid = atoi(valstr);
            }

            sprintf_s(str, 512, "DriverInfo:Drivers:CarIdx:{%d}CarNumberRaw:", i);
            if (parseYaml(yaml, str, &tVal, &tValLen))
            {
                int len = tValLen;
                if (len > 512)
                    len = 512;

                // copy what we can, even if buffer too small
                memcpy(valstr, tVal, len);
                valstr[len] = '\0'; // original string has no null termination...
                cState->number = atoi(valstr);
            }

            ret[cState->idx] = cState;

            ++i;
        }
        else
        {
            i = -1;
        }
    } while (i != -1);

    return ret;
}

IrTelemetrySystem::~IrTelemetrySystem()
{
}

void IrTelemetrySystem::configure(class ECS::World *world)
{
    world->subscribe<OnCameraChangeRequest>(this);
    world->subscribe<OnFrameNumChangeRequest>(this);
}

void IrTelemetrySystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void IrTelemetrySystem::receive(ECS::World *world, const OnFrameNumChangeRequest &event)
{
    // irsdk_broadcastMsg(irsdk_BroadcastReplaySearch, irsdk_RpySrch_ToStart, 0);

    irsdk_broadcastMsg(irskd_BroadcastReplaySetPlayPosition, irsdk_RpyPos_Begin, event.frameNum);

    int actual = 0;
    while (event.frameNum != actual)
    {
        irsdkClient::instance().waitForData(16);
        actual = g_replayFrameNum.getInt();
        Sleep(100);
    }
    irsdk_broadcastMsg(irsdk_BroadcastReplaySetPlaySpeed, 1, false, 0);
}

void IrTelemetrySystem::receive(ECS::World *world, const OnCameraChangeRequest &event)
{

    int requestedCarIdx = event.targetCarIdx;
    int requestedCarNum = 0;
    std::string requestedCarName;

    int camGroup = g_camGroup.getInt();

    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();

            int i = cState->idx;

            if (cState->idx == requestedCarIdx)
            {
                requestedCarNum = cState->number;
            }
        });

    if (SpecialCarNum::LEADER == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtLeader;
    }
    else if (SpecialCarNum::EXITING == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtExiting;
    }
    else if (SpecialCarNum::INCIDENT == requestedCarIdx)
    {
        requestedCarNum = irsdk_csMode::irsdk_csFocusAtIncident;
    }

    irsdk_broadcastMsg(irsdk_BroadcastCamSwitchNum, requestedCarNum, camGroup, 0);
    Sleep(100);
}

void IrTelemetrySystem::tick(class ECS::World *world, float deltaTime)
{
    static int first = 1;
    static int lastCam = -1;
    static float tSinceCamChange = 0;
    static float tSinceIrData = 0;
    static std::map<int, std::string> sessionNameMap;
    static std::map<int, int> sessionLapsMap;
    static std::vector<std::map<int, int>> sessionResultsMap;
    static int maxSessionIdx = -1;
    static int subsessionId = -1;

    tSinceCamChange += deltaTime;
    tSinceIrData += deltaTime;

    // wait up to 16 ms for start of session or new data
    if (irsdkClient::instance().waitForData(16))
    {
        // and grab the data
        if (first)
        {
            first = 0;

            auto sessionInfoStr = irsdk_getSessionInfoStr();

            auto cStates = getStaticCarStates(sessionInfoStr);
            sessionNameMap = getSessionNameMap(sessionInfoStr);
            sessionLapsMap = getSessionLapCountMap(sessionInfoStr);
            sessionResultsMap = getSessionResultsMap(sessionInfoStr);
            subsessionId = getSubsessionId(sessionInfoStr);

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
                    broadcastCarState.get()->idx = pair.second->idx;
                    broadcastCarSummary.get()->idx = pair.second->idx;
                }
            }
        }

        world->each<DynamicCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
            {
                DynamicCarStateComponentSP cState = cStateH.get();

                int i = cState->idx;

                if (i >= 0)
                {
                    float oldLapPct = cState->lapDistPct;
                    cState->lapDistPct = g_CarIdxLapDistPct.getFloat(i);
                    cState->officialPos = (int)g_carIdxClassPosition.getFloat(i);
                    cState->isInPits = ((int)g_isCarInPits.getFloat(i)) == irsdk_TrkLoc::irsdk_InPitStall;

                    int newLap = g_carIdxLap.getInt(i);
                    if (cState->currentLap < newLap)
                    {
                        // sometimes iRacing sends bad data when the car is crossing the
                        // start finish line, handle it here
                        cState->lapDistPct = 0;
                    }

                    cState->currentLap = newLap;

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
        sessionComp->subsessionId = subsessionId;
        if (sessionComp->num != g_sessionNum.getInt() - maxSessionIdx)
        {
            sessionComp->num = g_sessionNum.getInt() - maxSessionIdx;
            sessionComp->name = sessionNameMap[sessionComp->num];
            sessionComp->lapCount = sessionLapsMap[sessionComp->num];

            SessionResultComponentSP sessionResultComp = ECSUtil::getFirstCmp<SessionResultComponentSP>(world);

            if (g_sessionNum.getInt() >= 0)
            {
                sessionResultComp->carIndex2Position = sessionResultsMap[g_sessionNum.getInt()];
            }
        }

        tSinceIrData = 0;

        CameraActualsComponentSP cameraActualsCmp = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
        cameraActualsCmp->timeSinceLastChange = tSinceCamChange;
        cameraActualsCmp->replayFrameNum = g_replayFrameNum.getInt();
        cameraActualsCmp->replayFrameNumEnd = g_replayFrameNumEnd.getInt();
        cameraActualsCmp->currentCarIdx = g_camCarIdx.getInt();
        if (cameraActualsCmp->currentCarIdx != lastCam)
        {
            tSinceCamChange = 0;
            lastCam = cameraActualsCmp->currentCarIdx;
        }
    }

    // your normal process loop would go here
    monitorConnectionStatus();
}
