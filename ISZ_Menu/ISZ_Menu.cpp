#include <windows.h>

// Global Variables
HMODULE g_hModule = NULL;  // Save DLL module handle
HWND    g_hWnd = NULL;
bool    g_bShouldClose = false;

// Forward declaration of the Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Create and Show the GUI Window
DWORD WINAPI GuiThread(LPVOID lpParameter)
{
    // 1. Register Window Class using the module handle from DllMain
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hModule;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = TEXT("ISZ_MenuWindow");

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, TEXT("Failed to register window class"), TEXT("Error"), MB_ICONERROR);
        return 1;
    }

    // 2. Create Window
    g_hWnd = CreateWindow(
        TEXT("ISZ_MenuWindow"),
        TEXT("ISZ Menu"),
        WS_OVERLAPPEDWINDOW,
        100, 100, 400, 300,
        NULL, NULL, g_hModule, NULL);

    if (!g_hWnd)
    {
        MessageBox(NULL, TEXT("Failed to create window"), TEXT("Error"), MB_ICONERROR);
        UnregisterClass(TEXT("ISZ_MenuWindow"), g_hModule);
        return 1;
    }

    // 3. Show Window
    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);

    // 4. Message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (g_bShouldClose)
            break;
    }

    // Cleanup
    DestroyWindow(g_hWnd);
    UnregisterClass(TEXT("ISZ_MenuWindow"), g_hModule);
    return 0;
}

// DllMain (Entry Point)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;  // Save our module handle for later use
        DisableThreadLibraryCalls(hModule);

        // Start the GUI thread
        HANDLE hThread = CreateThread(NULL, 0, GuiThread, NULL, 0, NULL);
        if (hThread)
            CloseHandle(hThread);
    }
    return TRUE;
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Create a Button
        HWND hButton = CreateWindow(
            TEXT("BUTTON"),
            TEXT("Click Me"),
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
        if (LOWORD(wParam) == 1)  // Button Clicked
        {
            MessageBox(hWnd, TEXT("Button Clicked!"), TEXT("Info"), MB_OK);
        }
        break;
    }
    case WM_CLOSE:
    {
        g_bShouldClose = true;
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
