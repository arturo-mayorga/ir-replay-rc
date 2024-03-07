#ifndef scraper_sys_h_
#define scraper_sys_h_

#include "../ecs.h"
#include "../events/scraper-events.h"

#include <fstream>
#include <iostream>

struct Telemetry
{
    double percentPos;
    double percentPosDelta;
    int time;
};
typedef std::shared_ptr<Telemetry> TelemetrySP;

struct Lap
{
    int lapNumber;
    std::vector<TelemetrySP> telemetry;
};
typedef std::shared_ptr<Lap> LapSP;

struct Driver
{
    int uid;
    std::vector<LapSP> laps;
};
typedef std::shared_ptr<Driver> DriverSP;

struct EpochTelemetryDriverDatum
{
    int uid;
    double percentPos;
    double percentPosDelta;
};
typedef std::shared_ptr<EpochTelemetryDriverDatum> EpochTelemetryDriverDatumSP;

struct EpochTelemetryData
{
    int time;
    std::vector<EpochTelemetryDriverDatumSP> data;
};
typedef std::shared_ptr<EpochTelemetryData> EpochTelemetryDataSP;

struct EpochTelemetry
{
    int numLaps;
    int checkeredFlagTime;
    std::vector<EpochTelemetryDataSP> epochList;

    EpochTelemetry() : numLaps(0), checkeredFlagTime(0) {}
};
typedef std::shared_ptr<EpochTelemetry> EpochTelemetrySP;

struct Session
{
    int id;
    std::vector<DriverSP> drivers;
    EpochTelemetrySP epochTelemetry;
    Session() : epochTelemetry(new EpochTelemetry) {}
};
typedef std::shared_ptr<Session> SessionSP;

class ScraperSystem : public ECS::EntitySystem,
                      public ECS::EventSubscriber<OnSaveTelemetryRequest>
{
protected:
    std::vector<SessionSP> _sessions;
    std::ofstream _ofstream;
    SessionSP _currentSession;
    int _subsessionId = -1;
    int _isFinished = 0;

public:
    virtual ~ScraperSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    virtual void receive(ECS::World *world, const OnSaveTelemetryRequest &event) override;

    virtual std::ostream &getOutStream();
    virtual void closeOutStream();
    int isFinished() { return _isFinished; }
};

#endif