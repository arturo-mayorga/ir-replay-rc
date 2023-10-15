#include "window-rendering-sys.h"
#include "../components/rendering-comp.h"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <functional>
#include <set>

TCHAR *WINDOW_RENDERING_SYS_CLASS_NAME = _T("window-rendering-sys");

int CALLBACK EnumFontFamExProc(
    ENUMLOGFONTEX *lpelfe,
    NEWTEXTMETRICEX *lpntme,
    DWORD FontType,
    LPARAM lParam)
{
    std::vector<std::wstring> *fonts = reinterpret_cast<std::vector<std::wstring> *>(lParam);
    fonts->push_back(lpelfe->elfLogFont.lfFaceName);
    return 1; // Continue enumeration
}

std::vector<std::wstring> EnumerateFonts()
{
    std::vector<std::wstring> fonts;

    HDC hdc = GetDC(NULL); // Get device context for the screen
    LOGFONT lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;

    EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumFontFamExProc, (LPARAM)&fonts, 0);

    ReleaseDC(NULL, hdc);

    return fonts;
}

std::shared_ptr<WindowInfo> openWindow(CanvasConfigComponentSP cfg, WindowRenderingSystem *windowRenderingSystemInstance)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    std::shared_ptr<WindowInfo> ret(new WindowInfo());

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        WINDOW_RENDERING_SYS_CLASS_NAME,
        _T("ir-replay"),
        WS_POPUP,
        cfg->x, cfg->y, cfg->w, cfg->h,
        NULL,
        NULL,
        hInstance,
        windowRenderingSystemInstance);

    ret->hwnd = hwnd;

    if (!hwnd)
    {
        return ret;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    COLORREF transparentColor = RGB(255, 0, 255); // This color will be fully transparent.
    BYTE alphaValue = (255 * 90) / 100;           // Semi-transparency for the rest of the window.

    // Set the attributes for the layered window.
    SetLayeredWindowAttributes(hwnd, transparentColor, alphaValue, LWA_COLORKEY | LWA_ALPHA);

    // std::vector<std::wstring> fonts = EnumerateFonts();
    // for (const auto &fontName : fonts)
    // {
    //     std::wcout << L"'" << fontName << L"'" << std::endl;
    // }

    return ret;
}

LRESULT WindowRenderingSystem::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Dimensions of the area you want to redraw
        int width = ps.rcPaint.right - ps.rcPaint.left;
        int height = ps.rcPaint.bottom - ps.rcPaint.top;

        // Create memory DC and bitmap
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        if (_hwndToEntityMap.find(hwnd) != _hwndToEntityMap.end())
        {
            ECS::Entity *ent = _hwndToEntityMap[hwnd];

            ent->with<FillRectsComponentSP>(
                [&](ECS::ComponentHandle<FillRectsComponentSP> rectC)
                {
                    for (auto rect : rectC.get()->rects)
                    {
                        COLORREF myColor = RGB(rect->r, rect->g, rect->b);
                        HBRUSH hBrush = CreateSolidBrush(myColor);

                        RECT rc = {rect->x, rect->y, rect->x + rect->w, rect->y + rect->h};
                        FillRect(memDC, &rc, hBrush);

                        DeleteObject(hBrush);
                    }
                });

            ent->with<TextsComponentSP>(
                [&](ECS::ComponentHandle<TextsComponentSP> txtC)
                {
                    for (auto txt : txtC.get()->texts)
                    {
                        LOGFONT lf = {};
                        lf.lfHeight = txt->h;
                        lf.lfWeight = FW_BOLD;

                        _tcscpy_s(lf.lfFaceName, _T("Myriad Pro"));
                        HFONT hFont = CreateFontIndirect(&lf);
                        HFONT hOldFont = (HFONT)SelectObject(memDC, hFont);

                        COLORREF textColor = RGB(txt->r, txt->g, txt->b);
                        SetTextColor(memDC, textColor);
                        SetBkMode(memDC, TRANSPARENT);

                        TextOut(memDC, txt->x, txt->y, txt->text.c_str(), (int)txt->text.length());

                        SelectObject(memDC, hOldFont);
                        DeleteObject(hFont);
                    }
                });
        }
        else
        {
            // TODO: time to delete the window here
        }

        // Copy (BitBlt) the memory DC content to the primary DC
        BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, width, height, memDC, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);

        return 0;
    }
    case WM_NCHITTEST:
        return HTTRANSPARENT; // Makes the window click-through

    case WM_CREATE:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_LBUTTONDOWN:
    {
        POINT pt;
        GetCursorPos(&pt);
        HWND hwndBelow = WindowFromPoint(pt);
        if (hwndBelow != hwnd && hwndBelow != NULL)
        {
            // Pass the mouse message to the window below.
            SendMessage(hwndBelow, uMsg, wParam, MAKELPARAM(pt.x, pt.y));
        }
        else
        {
            PostQuitMessage(0);
        }
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

WindowRenderingSystem::~WindowRenderingSystem()
{
}

void WindowRenderingSystem::configure(class ECS::World *world)
{
    std::shared_ptr<WindowInfo> ret(new WindowInfo());

    // TODO: class registration will be a problem when we need more than one window
    HINSTANCE hInstance = GetModuleHandle(NULL);

    _wc.lpfnWndProc = WindowRenderingSystem::staticWindowProc;
    _wc.hInstance = hInstance;
    _wc.lpszClassName = WINDOW_RENDERING_SYS_CLASS_NAME;
    RegisterClass(&_wc);

    _latestWorld = world;
}

void WindowRenderingSystem::unconfigure(class ECS::World *world)
{
    _latestWorld = world;
}

void WindowRenderingSystem::tick(class ECS::World *world, float deltaTime)
{
    _latestWorld = world;
    MSG msg = {};
    BOOL result;

    static float redrawTimeAcc = 0;

    redrawTimeAcc += deltaTime;

    while ((result = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) != 0)
    {
        if (msg.message == WM_QUIT)
        {
            std::cout << "WindowRenderingSystem::WM_QUIT" << std::endl;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (redrawTimeAcc > 1)
    {
        for (auto it = _hwndToEntityMap.begin(); it != _hwndToEntityMap.end(); ++it)
        {
            InvalidateRect(it->first, NULL, TRUE);
        }
        redrawTimeAcc = 0;
    }

    static std::set<int> openCanvasIds;

    for (ECS::Entity *ent : world->each<CanvasConfigComponentSP>())
    {
        ent->with<CanvasConfigComponentSP>(
            [&](ECS::ComponentHandle<CanvasConfigComponentSP> canvasConfig)
            {
                if (openCanvasIds.find(canvasConfig.get()->id) == openCanvasIds.end())
                {
                    openCanvasIds.insert(canvasConfig.get()->id);
                    auto windowInfo = openWindow(canvasConfig.get(), this);

                    _hwndToEntityMap[windowInfo->hwnd] = ent;
                }
            });
    }
}