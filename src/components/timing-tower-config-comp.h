#ifndef timing_tower_config_comp_h_
#define timing_tower_config_comp_h_

#include "../ecs.h"
#include <memory>

struct TimingTowerConfigComponent
{
    ECS_DECLARE_TYPE;

    TimingTowerConfigComponent() {}

    std::wstring league;
};
ECS_DEFINE_TYPE(TimingTowerConfigComponent);

#endif