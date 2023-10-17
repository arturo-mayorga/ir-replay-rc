#ifndef irtelemetry_fake_sys_h_
#define irtelemetry_fake_sys_h_

#include "../ecs.h"
#include "../components/cam-ctrl-comp.h"

class IrTelemetryFakeSystem : public ECS::EntitySystem,
                              public ECS::EventSubscriber<OnCameraChangeRequest>,
                              public ECS::EventSubscriber<OnFrameNumChangeRequest>
{
protected:
    int _currentCarIndex = 0;
    int _currentFrameNum = 0;
    int _totalFrames = 60 * 60 * /* minutes >> */ 60;
    int _numCars = 30;
    std::vector<float> _carSpeeds;
    std::vector<float> _carLapDistPcts;

public:
    virtual ~IrTelemetryFakeSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    virtual void receive(ECS::World *world, const OnCameraChangeRequest &event) override;

    virtual void receive(ECS::World *world, const OnFrameNumChangeRequest &event) override;
};

#endif