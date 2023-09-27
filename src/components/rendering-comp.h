#ifndef rendering_comp_h_
#define rendering_comp_h_

#include "../ecs.h"
#include <string>

struct CanvasConfigComponent
{
    ECS_DECLARE_TYPE;

    CanvasConfigComponent() : x(0), y(0), w(0), h(0)
    {
        id = CanvasConfigComponent::autoID++;
    }

    int x;
    int y;
    int w;
    int h;
    int id;

    static int autoID;
};
ECS_DEFINE_TYPE(CanvasConfigComponent);
typedef std::shared_ptr<CanvasConfigComponent> CanvasConfigComponentSP;

struct FillRectInfo
{

    FillRectInfo(int _x, int _y, int _w, int _h, int _r, int _g, int _b) : x(_x), y(_y), w(_w), h(_h), r(_r), g(_g), b(_b) {}

    int x;
    int y;
    int w;
    int h;
    int r;
    int g;
    int b;
};
typedef std::shared_ptr<FillRectInfo> FillRectInfoSP;

struct FillRectsComponent
{
    ECS_DECLARE_TYPE;
    std::vector<FillRectInfoSP> rects;
};

ECS_DEFINE_TYPE(FillRectsComponent);
typedef std::shared_ptr<FillRectsComponent> FillRectsComponentSP;

struct TextInfo
{
    TextInfo(std::wstring _text, int _x, int _y, int _h, int _r, int _g, int _b) : text(_text), x(_x), y(_y), h(_h), r(_r), g(_g), b(_b) {}
    int x;
    int y;
    int h;
    int r;
    int g;
    int b;
    std::wstring text;
};
typedef std::shared_ptr<TextInfo> TextInfoSP;

struct TextsComponent
{
    ECS_DECLARE_TYPE;

    std::vector<TextInfoSP> texts;
}

ECS_DEFINE_TYPE(TextsComponent);
typedef std::shared_ptr<TextsComponent> TextsComponentSP;

#endif
