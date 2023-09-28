#include "current-driver-overlay-sys.h"

#include "../ecs-util.h"
#include "../strutil.h"

#include "../components/rendering-comp.h"
#include "../components/session-comp.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"

#include <sstream>
#include <map>

#include <string>
#include <locale>
#include <codecvt>

// inspiration from
// https://www.racedepartment.com/downloads/f1tv-tv-style-hud.53826/

const int HEIGHT = 47;
const int NUMBER_WIDTH = 47;
const int SEPARATOR_WIDTH = 5;
const int NAME_WIDTH = 296;

const int WIDTH = NUMBER_WIDTH + SEPARATOR_WIDTH + NAME_WIDTH;

struct CurrentDriverDisplayInfo
{
    int position;
    std::wstring name;
};

std::shared_ptr<CurrentDriverDisplayInfo> getCurrentDriverDisplayInfo(class ECS::World *world)
{
    std::shared_ptr<CurrentDriverDisplayInfo> ret(new CurrentDriverDisplayInfo());

#ifdef CURRENT_DRIVER_DISPLAY_TEST_MODE
    static int p = 0;

    ret->name = L"Antonio A Jimenez Tellez";
    ret->position = p;

    p = (p + 1) % 100;

#else
    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world).get();

    auto lInfo = ECSUtil::getFirstCmp<SessionLeaderBoardComponentSP>(world);

    int p = 1;
    for (auto car : lInfo->carsByPosition)
    {
        auto sCar = car->get<StaticCarStateComponentSP>().get();

        if (sCar->idx == cameraActualsComponent->currentCarIdx)
        {
            ret->name = convertToWide(sCar->name);
            break;
        }
        ++p;
    }
    ret->position = p;

#endif

    return ret;
}

CurrentDriverOverlaySystem::~CurrentDriverOverlaySystem()
{
}

void CurrentDriverOverlaySystem::configure(class ECS::World *world)
{
}
void CurrentDriverOverlaySystem::unconfigure(class ECS::World *world)
{
}
void CurrentDriverOverlaySystem::tick(class ECS::World *world, float deltaTime)
{
    if (NULL == _canvasEnt)
    {
        createWindow(world);
    }

    if (_canvasEnt)
    {
        updateTables(world);
    }
}

void CurrentDriverOverlaySystem::createWindow(ECS::World *world)
{
    _canvasEnt = world->create();

    auto cCfg = _canvasEnt->assign<CanvasConfigComponentSP>(new CanvasConfigComponent());
    cCfg.get()->x = 1375;
    cCfg.get()->y = 60; // 950
    cCfg.get()->w = WIDTH;
    cCfg.get()->h = HEIGHT;

    auto rect = _canvasEnt->assign<FillRectsComponentSP>(new FillRectsComponent());
    auto t1 = _canvasEnt->assign<TextsComponentSP>(new TextsComponent());
}

void CurrentDriverOverlaySystem::updateTables(ECS::World *world)
{
    auto rect = _canvasEnt->get<FillRectsComponentSP>();
    auto t1 = _canvasEnt->get<TextsComponentSP>();
    auto disp = getCurrentDriverDisplayInfo(world);

    rect.get()->rects.clear();
    t1.get()->texts.clear();

    int x = 0;
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, 0, NUMBER_WIDTH, HEIGHT, 206, 206, 206)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(x += NUMBER_WIDTH, 0, SEPARATOR_WIDTH, HEIGHT, 255, 0, 255)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(x += SEPARATOR_WIDTH, 0, NAME_WIDTH, HEIGHT, 206, 206, 206)));

    x = 0;
    std::wstringstream pos;
    pos << disp->position;
    if (disp->position >= 10)
    {
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(pos.str(), 2, 0, HEIGHT, 19, 19, 21)));
    }
    else
    {
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(pos.str(), 13, 0, HEIGHT, 19, 19, 21)));
    }

    t1.get()->texts.push_back(TextInfoSP(new TextInfo(trimString(disp->name, 15), x += NUMBER_WIDTH + SEPARATOR_WIDTH + 3, 0, HEIGHT, 19, 19, 21)));
}