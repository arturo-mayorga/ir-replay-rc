#ifndef session_leaderboard_sys_h_
#define session_leaderboard_sys_h_

#include "../ecs.h"

#include <vector>

class SessionLeaderBoardSystem : public ECS::EntitySystem
{
private:
public:
    virtual ~SessionLeaderBoardSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif