#ifndef window_rendering_sys_h_
#define window_rendering_sys_h_

#include "../ecs.h"

#include <map>

#include <windows.h>

struct WindowInfo
{
    WindowInfo() : wc(new WNDCLASS()), hwnd(NULL) {}
    std::shared_ptr<WNDCLASS> wc;
    HWND hwnd;
};

class WindowRenderingSystem : public ECS::EntitySystem
{
private:
    std::shared_ptr<WindowInfo> _tWindowInfo;
    std::map<HWND, ECS::Entity*> _hwndToEntityMap;

    ECS::World *_latestWorld;

public:
    virtual ~WindowRenderingSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK staticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }

        WindowRenderingSystem *pThis = (WindowRenderingSystem *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (pThis)
        {
            return pThis->windowProc(hwnd, uMsg, wParam, lParam);
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
};

#endif