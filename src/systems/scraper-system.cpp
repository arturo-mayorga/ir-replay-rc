#include "scraper-system.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/session-comp.h"
#include <map>
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>

#include "../ecs-util.h"

ScraperSystem::~ScraperSystem()
{
}

void ScraperSystem::configure(class ECS::World *world)
{
    world->subscribe<OnSaveTelemetryRequest>(this);
}

void ScraperSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void wtab(std::ostream &fstream, int depth, std::string tab)
{
    for (int i = 0; i < depth; ++i)
    {
        fstream << tab;
    }
}

template <typename T>
void writeArray(
    std::string name,
    std::vector<T> ta,
    void (*writer)(T, std::ostream &, std::string, int, std::string, std::string),
    std::ostream &fstream,
    std::string separator,
    int depth,
    std::string tab,
    std::string endl)
{
    wtab(fstream, depth++, tab);

    if (name.length() == 0)
    {
        fstream << "[" << endl;
    }
    else
    {
        fstream << "\"" << name << "\":[" << endl;
    }

    size_t tCount = ta.size();

    int i = 0;
    for (auto t : ta)
    {
        std::string sep = ",";
        if (i == tCount - 1)
        {
            sep = "";
        }
        ++i;

        writer(t, fstream, sep, depth, tab, endl);
    }

    wtab(fstream, --depth, tab);
    fstream << "]" << separator << endl;
}

void writeTelemetry(TelemetrySP telem, std::ostream &fstream, std::string separator, int depth, std::string tab, std::string endl)
{
    wtab(fstream, depth++, tab);
    fstream << "{"
            << "\"perc\":" << telem->percentPos << ",\"percD\":";
    if (std::isfinite(telem->percentPosDelta))
    {
        fstream << telem->percentPosDelta;
    }
    else
    {
        fstream << "null";
    }
    fstream << ",\"t\":" << telem->time << "}" << separator << endl;
}

void writeLap(LapSP lap, std::ostream &fstream, std::string separator, int depth, std::string tab, std::string endl)
{
    wtab(fstream, depth++, tab);
    fstream << "{" << endl;

    wtab(fstream, depth, tab);
    fstream << "\"lapNumber\":" << lap->lapNumber << "," << endl;

    writeArray<TelemetrySP>("telemetry", lap->telemetry, writeTelemetry, fstream, "", depth, tab, endl);

    wtab(fstream, --depth, tab);
    fstream << "}" << separator << endl;
}

void writeDriver(DriverSP driver, std::ostream &fstream, std::string separator, int depth, std::string tab, std::string endl)
{
    wtab(fstream, depth++, tab);
    fstream << "{" << endl;

    wtab(fstream, depth, tab);
    fstream << "\"id\":" << driver->uid << "," << endl;

    writeArray<LapSP>("laps", driver->laps, writeLap, fstream, "", depth, tab, endl);

    wtab(fstream, --depth, tab);
    fstream << "}" << separator << endl;
}

void writeSession(SessionSP session, std::ostream &fstream, std::string separator, int depth, std::string tab, std::string endl)
{
    if (session->drivers.size() > 0)
    {
        wtab(fstream, depth++, tab);
        fstream << "{" << endl;

        wtab(fstream, depth, tab);
        fstream << "\"id\":" << session->id << "," << endl;

        writeArray<DriverSP>("drivers", session->drivers, writeDriver, fstream, "", depth, tab, endl);

        wtab(fstream, --depth, tab);
        fstream << "}" << separator << endl;
    }
}

std::ostream &ScraperSystem::getOutStream()
{
    std::stringstream fileName;

    fileName << _subsessionId << ".json";
    this->_ofstream.open(fileName.str());
    return this->_ofstream;
}

void ScraperSystem::closeOutStream()
{
    this->_ofstream.close();
}

void ScraperSystem::receive(ECS::World *world, const OnSaveTelemetryRequest &event)
{
    std::ostream &fstream = this->getOutStream();

    // writeArray<SessionSP>("", _sessions, writeSession, fstream, "", 0, "    ", "\n");
    writeArray<SessionSP>("", _sessions, writeSession, fstream, "", 0, "", "");

    this->closeOutStream();

    std::cout << "scraping complete" << std::endl;
    _isFinished = true;
}

void ScraperSystem::tick(class ECS::World *world, float deltaTime)
{
    static int first = 1;
    static int currentSessionNum = -999;
    static std::map<int, int> driverIdx2uid;
    static std::map<int, double> uid2trackPercent;
    static std::map<int, int> uid2currentLapNum;
    static std::map<int, LapSP> currentLapMap;
    static std::map<int, DriverSP> currentDriverMap;

    if (first)
    {
        world->each<StaticCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
            {
                first = 0;
                StaticCarStateComponentSP cState = cStateH.get();
                driverIdx2uid[cState->idx] = cState->uid;

                std::cout << "driver map " << cState->idx << " " << cState->uid << "\n";
            });

        std::cout << "ready to scrape" << std::endl;

        return;
    }

    auto sessionComponent = ECSUtil::getFirstCmp<SessionComponentSP>(world);
    _subsessionId = sessionComponent->subsessionId;

    if (currentSessionNum != sessionComponent->num)
    {

        currentSessionNum = sessionComponent->num;

        _currentSession = SessionSP(new Session());
        _currentSession->id = currentSessionNum;

        _sessions.push_back(_currentSession);

        world->each<StaticCarStateComponentSP>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
            {
                StaticCarStateComponentSP cState = cStateH.get();

                if (cState->uid > 0)
                {

                    DriverSP driver(new Driver());
                    driver->uid = cState->uid;

                    _currentSession->drivers.push_back(driver);

                    currentDriverMap[cState->uid] = driver;
                    uid2trackPercent[cState->uid] = -1;
                    uid2currentLapNum[cState->uid] = -1;
                }
            });
    }

    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();
            int uid = driverIdx2uid[cState->idx];

            if (uid2currentLapNum[uid] < cState->currentLap)
            {
                LapSP lap(new Lap());
                lap->lapNumber = cState->currentLap;
                currentLapMap[uid] = lap;

                uid2currentLapNum[uid] = cState->currentLap;
                uid2trackPercent[uid] = -1;

                if (currentDriverMap[uid].get())
                {
                    currentDriverMap[uid]->laps.push_back(lap);
                }
                else
                {
                }
            }

            if (floor(cState->lapDistPct * 100) > floor(uid2trackPercent[uid]))
            {
                uid2trackPercent[uid] = 100 * cState->lapDistPct;

                TelemetrySP telemetry(new Telemetry());
                telemetry->percentPos = cState->lapDistPct;
                telemetry->percentPosDelta = cState->deltaLapDistPct;
                telemetry->time = cameraActualsComponent->replayFrameNum;

                if (currentLapMap[uid].get())
                {
                    currentLapMap[uid]->telemetry.push_back(telemetry);
                }
            }
        });

    if (cameraActualsComponent->replayFrameNumEnd == 1)
    {
        world->emit<OnSaveTelemetryRequest>(OnSaveTelemetryRequest());
    }
}
