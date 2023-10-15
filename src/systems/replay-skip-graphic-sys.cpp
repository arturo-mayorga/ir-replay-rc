#include "replay-skip-graphic-sys.h"
#include "../components/rendering-comp.h"
#include "../components/car-comp.h"
#include "../components/session-comp.h"

#include "../ecs-util.h"
#include "../strutil.h"

#include <iostream>
#include <sstream>
#include <map>

#include <string>

#define SCALE .3
#define GRAPHIC_WIDTH ((int)(3840 * SCALE))
#define GRAPHIC_HEIGHT ((int)(2160 * SCALE))

ReplaySkipGraphicSystem::~ReplaySkipGraphicSystem()
{
}

void ReplaySkipGraphicSystem::createWindow(ECS::World *world)
{
    // canvas comp
    _canvasEnt = world->create();

    auto cCfg = _canvasEnt->assign<CanvasConfigComponentSP>(new CanvasConfigComponent());
    cCfg.get()->x = 0;
    cCfg.get()->y = 0;
    cCfg.get()->w = GRAPHIC_WIDTH;
    cCfg.get()->h = GRAPHIC_HEIGHT;

    auto rect = _canvasEnt->assign<FillRectsComponentSP>(new FillRectsComponent());
    auto t1 = _canvasEnt->assign<TextsComponentSP>(new TextsComponent());
}

void ReplaySkipGraphicSystem::updateTables(ECS::World *world, float deltaTime)
{
    static float i = 0.75;
    auto rect = _canvasEnt->get<FillRectsComponentSP>();
    auto t1 = _canvasEnt->get<TextsComponentSP>();

    rect.get()->rects.clear();
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, 0, GRAPHIC_HEIGHT, GRAPHIC_HEIGHT, 255, 0, 255)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, 0, (int)(GRAPHIC_HEIGHT * (deltaTime / 200)), GRAPHIC_HEIGHT, 22, 22, 24)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(-1 * GRAPHIC_HEIGHT, 0, (int)(GRAPHIC_HEIGHT * (deltaTime / 400)), GRAPHIC_HEIGHT, 255, 0, 255)));

    t1.get()->texts.clear();

    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 10; ++x)
        {
            t1.get()->texts.push_back(TextInfoSP(new TextInfo(
                L"iFL",
                (int)(GRAPHIC_HEIGHT * (deltaTime / 1800)) + (int)(x * GRAPHIC_HEIGHT / 5) + y % 2 * (int)(GRAPHIC_HEIGHT / 10),
                (int)(y * GRAPHIC_HEIGHT / 5), (int)(GRAPHIC_HEIGHT / 10),
                255, 0, 255)));
        }
    }

    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"iFL", (int)(GRAPHIC_HEIGHT * (deltaTime / 700)), 0, (int)(GRAPHIC_HEIGHT + GRAPHIC_HEIGHT * (deltaTime / 10000)), 121, 121, 123)));
}

void ReplaySkipGraphicSystem::configure(class ECS::World *world)
{
    _canvasEnt = NULL;
}

void ReplaySkipGraphicSystem::unconfigure(class ECS::World *world)
{
}

void ReplaySkipGraphicSystem::tick(class ECS::World *world, float deltaTime)
{
    static float start = 0;

    if (NULL == _canvasEnt)
    {
        createWindow(world);
    }

    start += deltaTime;

    if (_canvasEnt)
    {
        updateTables(world, start);
    }
}