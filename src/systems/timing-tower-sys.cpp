#include "timing-tower-sys.h"

#include <sstream>

#include "../components/rendering-comp.h"

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

std::vector<TowerEntrySP> getTowerEntries()
{
    std::vector<TowerEntrySP> ret;

    ret.push_back(TowerEntrySP(new TowerEntry(L"1", L"444", L"Qio-Tiago Marteau", L"Leader", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"2", L"444", L"Jesper Sandström", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"3", L"444", L"Kenzy Nieuwhof", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"4", L"444", L"Aaron J Smith", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"5", L"444", L"Samuel Hermer", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"6", L"444", L"Christian Glåmseter", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"7", L"444", L"Evan M Waay", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"8", L"444", L"Niklas Wessman", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"9", L"444", L"Andy Flint", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"10", L"444", L"Antonio A Jimenez Tellez", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"11", L"444", L"Teemu Myllykangas", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"12", L"444", L"Robert Ridgway", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"13", L"444", L"Chuck Mauro", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"14", L"444", L"Sjaak Willems", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"15", L"444", L"Conor D Gibb", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"16", L"444", L"Philipp Lukas", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"17", L"444", L"Zoltán Zomotor", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"18", L"444", L"Steven Ridders", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"19", L"444", L"Paolo Breme", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"20", L"444", L"Arturo Mayorga", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"21", L"444", L"Mareks Ortlovs", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"22", L"444", L"Ryan Berry", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"23", L"444", L"Garret Spenst", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"24", L"444", L"Marcis Putnieks", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"25", L"444", L"Vyacheslav Potapenko", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"26", L"444", L"Adam Talsma", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"27", L"444", L"Rino Scattini", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"28", L"444", L"Dorian Arroyo", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"29", L"444", L"Michael LeMay", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"30", L"444", L"Ben Goyette", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"31", L"444", L"Leonard O Cheri", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"32", L"444", L"Matthew Breuer", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"33", L"444", L"Kouji Itoh", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"34", L"444", L"Nick Carty", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"35", L"444", L"Alvin Chan", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"36", L"444", L"Reade Dennis", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"37", L"444", L"Patrick Rowlands", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"38", L"444", L"Leo Ho Yin Leung", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"39", L"444", L"Max Bartolotta", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"40", L"444", L"Simon FESSY2", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"41", L"444", L"Cesar Martin", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"42", L"444", L"Fede Rosano", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"43", L"444", L"Marcello Keßler", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"44", L"444", L"Mauro Micheletti", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"45", L"444", L"Maximilian Gebhardt", L"+2.372", L"0", 1)));
    ret.push_back(TowerEntrySP(new TowerEntry(L"46", L"444", L"Clarke Williams", L"+100.372", L"0", 1)));

    return ret;
}

TimingTowerSystem::~TimingTowerSystem()
{
}

void TimingTowerSystem::configure(class ECS::World *world)
{
    // https://www.reddit.com/media?url=https%3A%2F%2Fpreview.redd.it%2Fwlpf4i86a2q81.jpg%3Fauto%3Dwebp%26s%3Db03ab7fbf9a632645515356569709d09507e9133
    auto entries = getTowerEntries();

    ColWidths w(10, 15, 25, 190, 65, 10);
    int h = 20;

    // canvas comp
    ECS::Entity *ent = world->create();

    int headerHeight = w.m + 55;
    auto cCfg = ent->assign<CanvasConfigComponentSP>(new CanvasConfigComponent());
    cCfg.get()->x = 20;
    cCfg.get()->y = 20;
    cCfg.get()->w = w.pos + w.num + w.nam + w.del + w.pit + (6 * w.m);
    cCfg.get()->h = (int)entries.size() * h + w.m + headerHeight;

    auto rect = ent->assign<FillRectsComponentSP>(new FillRectsComponent());
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, 0, cCfg.get()->w, headerHeight, 22, 22, 24)));
    rect.get()->rects.push_back(FillRectInfoSP(new FillRectInfo(0, headerHeight, cCfg.get()->w, cCfg.get()->h, 19, 19, 21)));
    // title bg rgba(19,19,21
    // bg rgba(22,22,24,255)

    auto t1 = ent->assign<TextsComponentSP>(new TextsComponent());

    // league rgba(194,196,186
    // race rgba(154,155,150
    // lap rgba(121,121,123
    // 7 rgba(200,202,201

    int header1W = 133;
    int header2W = 97;

    int headerOffset = (int)((cCfg.get()->w - header1W) / 2);
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"iGPF", headerOffset, 0, (int)(h * 2), 194, 194, 186)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"RACE", headerOffset + (int)(h * 3.75), (int)(h * .35), (int)(h * 1.5), 154, 155, 150)));

    headerOffset = (int)((cCfg.get()->w - header2W) / 2);
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"LAP", headerOffset, h * 2, (int)(h * 1.15), 121, 121, 123)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"45", headerOffset + (int)(h * 2), h * 2, (int)(h * 1.25), 200, 202, 201)));
    t1.get()->texts.push_back(TextInfoSP(new TextInfo(L"/50", headerOffset + (int)(h * 3.25), h * 2, (int)(h * 1.15), 121, 121, 123)));

    int i = 0;
    int yo = headerHeight;
    for (auto e : entries)
    {
        int x = w.m;
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->p, x, yo + (h * i), h, 166, 166, 166)));
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->number, x += w.m + w.pos, yo + (h * i), h, 121, 121, 121)));
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->name, x += w.m + w.num, yo + (h * i), h, 206, 206, 206)));
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->delta, x += w.m + w.nam, yo + (h * i), h, 206, 206, 206)));
        t1.get()->texts.push_back(TextInfoSP(new TextInfo(e->stops, x += w.m + w.del, yo + (h * i), h, 121, 121, 121)));
        ++i;
    }
}

void TimingTowerSystem::unconfigure(class ECS::World *world)
{
}

void TimingTowerSystem::tick(class ECS::World *world, float deltaTime)
{
}