#include "current-driver-overlay-sys.h"

#include "../ecs-util.h"

#include "../components/rendering-comp.h"

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

std::map<int, std::string> getIdx2nameMap_CurrentDriverOverlaySystem(class ECS::World *world)
{
    std::map<int, std::string> idx2name;
    world->each<StaticCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<StaticCarStateComponentSP> cStateH)
        {
            StaticCarStateComponentSP cState = cStateH.get();
            idx2name[cState->idx] = cState->name;
        });
    return idx2name;
}

std::shared_ptr<CurrentDriverDisplayInfo> getCurrentDriverDisplayInfo(class ECS::World *world)
{
    std::shared_ptr<CurrentDriverDisplayInfo> ret(new CurrentDriverDisplayInfo());

#ifdef CURRENT_DRIVER_DISPLAY_TEST_MODE
    static int p = 0;

    ret->name = L"Antonio A Jimenez Tellez";
    ret->position = p;

    p = (p + 1) % 100;

#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

    auto cameraActualsComponent = ECSUtil::getFirstCmp<CameraActualsComponentSP>(world);
    auto idx2nameMap = getIdx2nameMap_CurrentDriverOverlaySystem(world);
    cameraActualsComponent.get()->currentCarIdx;
    ret->name = converter.from_bytes(idx2nameMap[cameraActualsComponent.get()->currentCarIdx]);

    std::vector<DynamicCarStateComponentSP> states;
    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            states.push_back(cState);
        });
    // sort the states by track position and current lap
    std::sort(states.begin(), states.end(), [](DynamicCarStateComponentSP &a, DynamicCarStateComponentSP &b)
              { return (a->currentLap == b->currentLap) ? a->lapDistPct > b->lapDistPct : a->currentLap > b->currentLap; });
    int p = 1;
    for (auto s : states)
    {
        if (s->idx == cameraActualsComponent.get()->currentCarIdx)
        {
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

    // canvas comp
    _canvasEnt = world->create();

    auto cCfg = _canvasEnt->assign<CanvasConfigComponentSP>(new CanvasConfigComponent());
    cCfg.get()->x = 1375;
    cCfg.get()->y = 60; // 950
    cCfg.get()->w = WIDTH;
    cCfg.get()->h = HEIGHT;

    auto rect = _canvasEnt->assign<FillRectsComponentSP>(new FillRectsComponent());
    auto t1 = _canvasEnt->assign<TextsComponentSP>(new TextsComponent());
}

std::wstring trimString(const std::wstring &input, int maxLen)
{
    if (input.size() <= maxLen)
    {
        return input;
    }

    return input.substr(0, maxLen - 1).append(L"...");
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