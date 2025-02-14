#include <Windows.h>

// -------------------------------------------------------------------------
// Global Variables for GUI
// -------------------------------------------------------------------------
HMODULE g_hModule = NULL;
HWND    g_hWnd    = NULL;

// -------------------------------------------------------------------------
// Global Variables for DXGI Proxy
// -------------------------------------------------------------------------
static HMODULE g_hRealDxgi = nullptr;

// -------------------------------------------------------------------------
// Load the real dxgi.dll from System32
// -------------------------------------------------------------------------
static void LoadRealDxgi()
{
    if (!g_hRealDxgi)
    {
        char sysDir[MAX_PATH];
        GetSystemDirectoryA(sysDir, MAX_PATH);
        strcat_s(sysDir, "\\dxgi.dll");
        g_hRealDxgi = LoadLibraryA(sysDir);
    }
}

// -------------------------------------------------------------------------
// Window Procedure for our GUI window
// -------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Create a simple button in the window
        HWND hButton = CreateWindow(
            TEXT("BUTTON"), TEXT("Click Me"),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            50, 50, 100, 30,
            hWnd, (HMENU)1, g_hModule, NULL);
        if (!hButton)
        {
            MessageBox(hWnd, TEXT("Failed to create button"), TEXT("Error"), MB_ICONERROR);
        }
        break;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1)  // Button click
        {
            MessageBox(hWnd, TEXT("Button Clicked!"), TEXT("Info"), MB_OK);
        }
        break;
    }
    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// -------------------------------------------------------------------------
// GUI Thread Function: Registers a window class, creates a window, and
// runs a message loop so the window remains visible.
// -------------------------------------------------------------------------
DWORD WINAPI GuiThread(LPVOID lpParameter)
{
    // 1. Register Window Class
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = g_hModule;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = TEXT("ISZ_MenuWindow");

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Failed to register window class"), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

    // 2. Create the Window
    g_hWnd = CreateWindow(
        TEXT("ISZ_MenuWindow"),
        TEXT("ISZ Menu"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, g_hModule, NULL);
    if (!g_hWnd)
    {
        MessageBox(NULL, TEXT("Failed to create window"), TEXT("Error"), MB_ICONERROR);
        UnregisterClass(TEXT("ISZ_MenuWindow"), g_hModule);
        return 1;
    }

    // 3. Show the Window
    ShowWindow(g_hWnd, SW_SHOW);
    UpdateWindow(g_hWnd);

    // 4. Message Loop: keeps the window responsive and persistent
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(TEXT("ISZ_MenuWindow"), g_hModule);
    return 0;
}

// -------------------------------------------------------------------------
// DllMain: Entry point for our DLL proxy
// -------------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        g_hModule = hModule;  // Save our own module handle for later use
        DisableThreadLibraryCalls(hModule);

        // Start the GUI thread with a persistent message loop
        HANDLE hThread = CreateThread(nullptr, 0, GuiThread, nullptr, 0, nullptr);
        if (hThread)
        {
            CloseHandle(hThread);
        }

        // Load the real dxgi.dll so our proxy can forward calls
        LoadRealDxgi();

        break;
    }
    case DLL_PROCESS_DETACH:
    {
        if (g_hRealDxgi)
        {
            FreeLibrary(g_hRealDxgi);
            g_hRealDxgi = nullptr;
        }
        break;
    }
    }
    return TRUE;
}

// -------------------------------------------------------------------------
// DXGI Forwarding Functions: Exported functions that forward calls to the
// real dxgi.dll
// -------------------------------------------------------------------------

// 1) CreateDXGIFactory
typedef HRESULT(WINAPI* tCreateDXGIFactory)(REFIID, void**);
extern "C" __declspec(dllexport)
HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory)
{
    LoadRealDxgi();
    static tCreateDXGIFactory realFn = (tCreateDXGIFactory)GetProcAddress(g_hRealDxgi, "CreateDXGIFactory");
    if (realFn)
        return realFn(riid, ppFactory);
    return E_FAIL;
}

// 2) CreateDXGIFactory1
typedef HRESULT(WINAPI* tCreateDXGIFactory1)(REFIID, void**);
extern "C" __declspec(dllexport)
HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory)
{
    LoadRealDxgi();
    static tCreateDXGIFactory1 realFn = (tCreateDXGIFactory1)GetProcAddress(g_hRealDxgi, "CreateDXGIFactory1");
    if (realFn)
        return realFn(riid, ppFactory);
    return E_FAIL;
}

// 3) CreateDXGIFactory2
typedef HRESULT(WINAPI* tCreateDXGIFactory2)(UINT, REFIID, void**);
extern "C" __declspec(dllexport)
HRESULT WINAPI CreateDXGIFactory2(UINT Flags, REFIID riid, void** ppFactory)
{
    LoadRealDxgi();
    static tCreateDXGIFactory2 realFn = (tCreateDXGIFactory2)GetProcAddress(g_hRealDxgi, "CreateDXGIFactory2");
    if (realFn)
        return realFn(Flags, riid, ppFactory);
    return E_FAIL;
}
