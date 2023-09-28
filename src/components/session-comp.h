#ifndef session_comp_h_
#define session_comp_h_

#include "../ecs.h"
#include <string>

struct SessionComponent
{
    ECS_DECLARE_TYPE;

    SessionComponent() : num(0), lapCount(0), name("") {}

    int num;
    int lapCount;
    std::string name;
};
ECS_DEFINE_TYPE(SessionComponent);
typedef std::shared_ptr<SessionComponent> SessionComponentSP;

struct SessionLeaderBoardComponent
{
    ECS_DECLARE_TYPE;

    SessionLeaderBoardComponent() : currentLap(0) {}

    int currentLap;
    std::vector<ECS::Entity *> carsByPosition;
};
ECS_DEFINE_TYPE(SessionLeaderBoardComponent);
typedef std::shared_ptr<SessionLeaderBoardComponent> SessionLeaderBoardComponentSP;

#endif