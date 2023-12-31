#ifndef cam_ctrl_comp_h_
#define cam_ctrl_comp_h_

#include "../ecs.h"
#include <memory>

enum SpecialCarNum
{
    // note that this doesn't match irsdk_csMode to ensure avoidint
    // cross dependencies
    EXITING = -3,
    INCIDENT = -2,
    LEADER = -1,
    // DRIVER + car number...
    DRIVER = 0
};

struct OnCameraChangeRequest
{
    ECS_DECLARE_TYPE;

    OnCameraChangeRequest(int _targetCarIdx) : targetCarIdx(_targetCarIdx) {}

    int targetCarIdx;
};

struct OnFrameNumChangeRequest
{
    ECS_DECLARE_TYPE;

    OnFrameNumChangeRequest(int _frameNum) : frameNum(_frameNum) {}

    int frameNum;
};

struct CameraActualsComponent
{
    ECS_DECLARE_TYPE;

    CameraActualsComponent() : currentCarIdx(-1), timeSinceLastChange(0), replayFrameNum(-1), replayFrameNumEnd(-1) {}

    int currentCarIdx;
    float timeSinceLastChange;
    int replayFrameNum;
    int replayFrameNumEnd;
};
ECS_DEFINE_TYPE(CameraActualsComponent);
typedef std::shared_ptr<CameraActualsComponent> CameraActualsComponentSP;

struct CameraDirectionSubTargetsComponent
{
    ECS_DECLARE_TYPE;

    CameraDirectionSubTargetsComponent() : closestBattleCarIdx(-1), tvPointsCarIdx(-1) {}

    int closestBattleCarIdx;
    int tvPointsCarIdx;
};
ECS_DEFINE_TYPE(CameraDirectionSubTargetsComponent);
typedef std::shared_ptr<CameraDirectionSubTargetsComponent> CameraDirectionSubTargetsComponentSP;

struct CameraScriptItem
{
    ECS_DECLARE_TYPE;

    CameraScriptItem(int time_, int driverId_) : time(time_), driverId(driverId_) {}

    int time;
    int driverId;
};
typedef std::shared_ptr<CameraScriptItem> CameraScriptItemSP;

struct CameraScriptComponent
{
    ECS_DECLARE_TYPE;

    CameraScriptComponent() : done(0) {}

    std::vector<CameraScriptItemSP> items;
    int done;
};
ECS_DEFINE_TYPE(CameraScriptComponent);
typedef std::shared_ptr<CameraScriptComponent> CameraScriptComponentSP;

#endif