// PowerRenameUIHost.cpp : Defines the entry point for the application.
//
#include "pch.h"

#include "XamlBridge.h"

#include "framework.h"
#include "PowerRenameUIHost.h"

#define MAX_LOADSTRING 100

const wchar_t c_WindowClass[] = L"PowerRename";

class AppWindow : public DesktopWindowT<AppWindow>
{
public:
    static int Show(HINSTANCE hInstance, int nCmdShow)
    {
        auto window = AppWindow(hInstance);
        window.CreateAndShowWindow(nCmdShow);
        return window.MessageLoop(window.m_accelerators.get());
    }

    LRESULT MessageHandler(UINT message, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch (message)
        {
            HANDLE_MSG(WindowHandle(), WM_CREATE, OnCreate);
            HANDLE_MSG(WindowHandle(), WM_COMMAND, OnCommand);
            HANDLE_MSG(WindowHandle(), WM_DESTROY, OnDestroy);
            HANDLE_MSG(WindowHandle(), WM_SIZE, OnResize);
        default:
            return base_type::MessageHandler(message, wParam, lParam);
        }

        return base_type::MessageHandler(message, wParam, lParam);
    }

private:
    AppWindow(HINSTANCE hInstance) noexcept :
        m_instance(hInstance)
    {
    }

    void CreateAndShowWindow(int nCmdShow)
    {
        m_accelerators.reset(LoadAcceleratorsW(m_instance, MAKEINTRESOURCE(IDC_POWERRENAMEUIHOST)));

        WNDCLASSEXW wcex = { sizeof(wcex) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = m_instance;
        wcex.hIcon = LoadIconW(m_instance, MAKEINTRESOURCE(IDC_POWERRENAMEUIHOST));
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_POWERRENAMEUIHOST);
        wcex.lpszClassName = c_WindowClass;
        wcex.hIconSm = LoadIconW(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        RegisterClassExW(&wcex); // don't test result, handle error at CreateWindow

        wchar_t title[64];
        LoadStringW(m_instance, IDS_APP_TITLE, title, ARRAYSIZE(title));

        HWND window = CreateWindowW(c_WindowClass, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, m_instance, this);
        THROW_LAST_ERROR_IF(!window);

        ShowWindow(window, nCmdShow);
        UpdateWindow(window);
        SetFocus(window);
    }

    bool OnCreate(HWND, LPCREATESTRUCT) noexcept
    {
        m_mainUserControl = winrt::PowerRenameUI_new::MainWindow();
        m_xamlIsland = CreateDesktopWindowsXamlSource(WS_TABSTOP, m_mainUserControl);

        return true;
    }

    void OnCommand(HWND, int id, HWND hwndCtl, UINT codeNotify) noexcept
    {
        switch (id)
        {
        case IDM_ABOUT:
            DialogBoxW(m_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), WindowHandle(), [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
                switch (message)
                {
                case WM_INITDIALOG:
                    return TRUE;

                case WM_COMMAND:
                    if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
                    {
                        EndDialog(hDlg, LOWORD(wParam));
                        return TRUE;
                    }
                    break;
                }
                return FALSE;
            });
            break;

        case IDM_EXIT:
            PostQuitMessage(0);
            break;
        }
    }

    void OnDestroy(HWND hwnd) noexcept
    {
        //m_buttonClickRevoker.revoke();
        base_type::OnDestroy(hwnd);
    }

    void OnResize(HWND, UINT state, int cx, int cy) noexcept
    {
        const auto newHeight = cy;
        const auto newWidth = cx;
        const auto islandHeight = newHeight - (50 * 2) - 10;
        const auto islandWidth = newWidth - (10 * 2);
        //SetWindowPos(m_button1.get(), 0, ButtonWidth * 2, ButtonMargin, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
        //SetWindowPos(m_xamlButton1, m_button1.get(), newWidth - (ButtonWidth * 2), ButtonMargin, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
        SetWindowPos(m_xamlIsland, NULL, 0, 60, islandWidth, islandHeight, SWP_SHOWWINDOW);
        //SetWindowPos(m_button2.get(), m_xamlIsland, (ButtonMargin + newWidth - ButtonWidth) / 2, newHeight - ButtonMargin - ButtonHeight, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
    }

    wil::unique_haccel m_accelerators;
    const HINSTANCE m_instance;
    HWND m_xamlIsland{};
    winrt::PowerRenameUI_new::MainWindow m_mainUserControl{ nullptr };
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    winrt::PowerRenameUI_new::App app;

    const auto result = AppWindow::Show(hInstance, nCmdShow);

    app.Close();

    return result;
}
