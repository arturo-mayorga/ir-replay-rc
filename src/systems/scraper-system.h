#ifndef scraper_sys_h_
#define scraper_sys_h_

#include "../ecs.h"
#include "../events/scraper-events.h"

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

struct Session
{
    int id;
    std::vector<DriverSP> drivers;
};
typedef std::shared_ptr<Session> SessionSP;

class ScraperSystem : public ECS::EntitySystem,
                      public ECS::EventSubscriber<OnSaveTelemetryRequest>
{
private:
    std::vector<SessionSP> _sessions;

public:
    virtual ~ScraperSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    virtual void receive(ECS::World *world, const OnSaveTelemetryRequest &event) override;
};

#endif