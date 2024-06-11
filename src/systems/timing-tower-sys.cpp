#include "timing-tower-sys.h"
#include "../components/rendering-comp.h"
#include "../components/car-comp.h"
#include "../components/session-comp.h"
#include "../components/timing-tower-config-comp.h"

#include "../ecs-util.h"
#include "../strutil.h"

#include <iostream>
#include <sstream>
#include <map>

#include <string>

// #define TEST_TOWER_MODE 1

struct TowerEntry
{
    TowerEntry(std::wstring _p,
               std::wstring _number,
               std::wstring _name,
               std::wstring _delta,
               std::wstring _stops,
               int _isActive) : p(_p), number(_number), name(_name), delta(_delta), stops(_stops), isActive(_isActive) {}

    std::wstring p;
    std::wstring number;
    std::wstring name;
    std::wstring delta;
    std::wstring stops;
    int isActive;
};
typedef std::shared_ptr<TowerEntry> TowerEntrySP;

struct ColWidths
{
    ColWidths(int _m, int _pos, int _num, int _nam, int _del, int _pit) : m(_m), pos(_pos), num(_num), nam(_nam), del(_del), pit(_pit) {}

    int m;
    int pos;
    int num;
    int nam;
    int del;
    int pit;
};

const ColWidths w(10, 15, 25, 130, 65, 10);
const int h = 20;
const int headerHeight = w.m + 60;

std::vector<TowerEntrySP> getTowerEntries(ECS::World *world)
{
    std::vector<TowerEntrySP> ret;
#ifdef TEST_TOWER_MODE

    ret.push_back(TowerEntrySP(new TowerEntry(L"   1", L"444", L"Qio-Tiago Marteau", L"Leader", L"0", 1)));

    for (int i = 2; i < 45; ++i)
    {
        std::wstringstream p;
        if (i < 10)
        {
            p << L"   ";
        }
        p << i;
        ret.push_back(TowerEntrySP(new TowerEntry(p.str(), L"444", L"Jesper Sandstrom", L"+2.372", L"0", 1)));
    }
#else
    auto lInfo = ECSUtil::getFirstCmp<SessionLeaderBoardComponentSP>(world);

    int i = 0;
    for (auto car : lInfo->carsByPosition)
    {
        auto dCar = car->get<DynamicCarStateComponentSP>().get();
        auto sCar = car->get<StaticCarStateComponentSP>().get();

        ++i;
        std::wstringstream pos;
        if (i < 10)
        {
            pos << L"   ";
        }
        pos << i;

        std::wstringstream number;
        number << sCar->number;

        ret.push_back(TowerEntrySP(
            new TowerEntry(
                pos.str(),
                number.str(),
                trimString(convertToWide(sCar->name), 15),
                L"???",
                L"?",
                1)));
    }
#endif

    return ret;
}

TimingTowerSystem::~TimingTowerSystem()
{
}

void TimingTowerSystem::createWindow(ECS::World *world)
{
    auto entries = getTowerEntries(world);

    if (entries.size() == 0)
    {
        return;
    }

    // canvas comp
    _canvasEnt = world->create();

    auto cCfg = _canvasEnt->assign<CanvasConfigComponentSP>(new CanvasConfigComponent());
    cCfg.get()->x = 120;
    cCfg.get()->y = 120;
    cCfg.get()->w = w.pos /*+ w.num*/ + w.nam /*+ w.del + w.pit*/ + (/*6*/ 3 * w.m);
    cCfg.get()->h = (int)entries.size() * h + w.m + headerHeight;

    auto rect = _canvasEnt->assign<FillRectsComponentSP>(new FillRectsComponent());
    auto t1 = _canvasEnt->assign<TextsComponentSP>(new TextsComponent());
}

void TimingTowerSystem::updateTables(ECS::World *world)
{
    // https://www.reddit.com/media?url=https%3A%2F%2Fpreview.redd.it%2Fwlpf4i86a2q81.jpg%3Fauto%3Dwebp%26s%3Db03ab7fbf9a632645515356569709d09507e9133

    auto entries = getTowerEntries(world);

    int wd = w.pos /*+ w.num*/ + w.nam /*+ w.del + w.pit*/ + (/*6*/ 3 * w.m);
    int ht = (int)entries.size() * h + w.m + headerHeight;

    auto rect = _canvasEnt->get<FillRectsComponentSP>();
    auto t1 = _canvasEnt->get<TextsComponentSP>();

    rect.get()->rects.clear();
    t1.get()->texts.clear();

    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, 0, wd, headerHeight, 22, 22, 24)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, headerHeight, wd, 5, 255, 0, 255)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, headerHeight + 5, wd, ht, 19, 19, 21)));
    // title bg rgba(19,19,21
    // bg rgba(22,22,24,255)

    // league rgba(194,196,186
    // race rgba(154,155,150
    // lap rgba(121,121,123
    // 7 rgba(200,202,201

    int header1W = 133;
    int header2W = 97;

    auto config = ECSUtil::getFirstCmp<TimingTowerConfigComponent>(world);

    int headerOffset = (int)((wd - header1W) / 2);
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(config.league, headerOffset, 0, (int)(h * 2), 194, 194, 186)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"RACE", headerOffset + (int)(h * 3.75), (int)(h * .35), (int)(h * 1.5), 154, 155, 150)));

    headerOffset = (int)((wd - header2W) / 2);

    int lapI = ECSUtil::getFirstCmp<SessionLeaderBoardComponentSP>(world)->currentLap;
    int slapI = ECSUtil::getFirstCmp<SessionComponentSP>(world)->lapCount;
    std::wstringstream currentLap;
    std::wstringstream sessionLaps;

    if (lapI > slapI && 0 != slapI)
    {
        currentLap << L"Finish";
    }
    else
    {
        if (lapI < 10)
        {
            currentLap << L"   ";
        }
        currentLap << lapI;

        if (0 == slapI)
        {
            sessionLaps << L"/Inf";
        }
        else
        {

            sessionLaps << L"/" << slapI;
        }
    }

    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"LAP", headerOffset, h * 2, (int)(h * 1.15), 121, 121, 123)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(currentLap.str(), headerOffset + (int)(h * 2), h * 2, (int)(h * 1.25), 200, 202, 201)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(sessionLaps.str(), headerOffset + (int)(h * 3.25), h * 2, (int)(h * 1.15), 121, 121, 123)));

    int i = 0;
    int yo = headerHeight + 5;
    for (auto e : entries)
    {
        int x = w.m;
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->p, x, yo + (h * i), h, 166, 166, 166)));
        // t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->number, x += w.m + w.pos, yo + (h * i), h, 121, 121, 121)));

        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->name, x += w.m + w.pos, yo + (h * i), h, 206, 206, 206)));

        // t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->name, x += w.m + w.num, yo + (h * i), h, 206, 206, 206)));
        // t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->delta, x += w.m + w.nam, yo + (h * i), h, 206, 206, 206)));
        // t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->stops, x += w.m + w.del, yo + (h * i), h, 121, 121, 121)));
        ++i;
    }
}

void TimingTowerSystem::configure(class ECS::World *world)
{
    _canvasEnt = NULL;
}

void TimingTowerSystem::unconfigure(class ECS::World *world)
{
}

void TimingTowerSystem::tick(class ECS::World *world, float deltaTime)
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