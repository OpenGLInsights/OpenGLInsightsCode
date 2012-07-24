#include "stdafx.h"
#include "window.h"
#include <Strsafe.h>
/// Constructor
window::window()
{
    _width = 640;
    _height = 480;
}

/// Destructor
window::~window()
{
}

/// Handle window resize
///
/// @param newWidth [in] 
/// @param newHeight [in] 
void window::resize(int newWidth, int newHeight)
{
    _width = newWidth;
    _height = newHeight;
}

/// Main Window Callback Function 
///
/// @param hWnd [in] 
/// @param uMsg [in] 
/// @param wParam [in] 
/// @param lParam [in] 
static LONG WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
    LONG lRet = 1;
    RECT rect;
    window *curWindow;
     
    switch (uMsg)
    {
    case WM_CREATE: 
        break; 
     
    case WM_PAINT: 
        break; 
     
    case WM_SIZE: 
        GetClientRect(hWnd, &rect);
        curWindow = (window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        curWindow->resize(rect.right - rect.left, rect.bottom - rect.top);
        break; 
     
    case WM_CLOSE: 
        PostQuitMessage(0);
        DestroyWindow(hWnd); 
        break; 
     
    case WM_DESTROY: 
        PostQuitMessage(0); 
        break; 
         
    case WM_KEYDOWN: 
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        } 
     
    default: 
        lRet = (LONG)DefWindowProc(hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet; 
} 

/// Create window.
///
/// @param hInstance [in] 
/// @param x [in] 
/// @param y [in] 
/// @param title [in] 
///
/// @return false if failed
bool window::setup(HINSTANCE hInstance, int x, int y, const char *title)
{
    WNDCLASS wndclass; 
    static const CHAR szAppName[]= "OpenGL Copy Engine performance test";

    // Register the frame class.
    wndclass.style         = 0; 
    wndclass.lpfnWndProc   = (WNDPROC)MainWndProc; 
    wndclass.cbClsExtra    = 0; 
    wndclass.cbWndExtra    = 0; 
    wndclass.hInstance     = hInstance; 
    wndclass.hIcon         = LoadIcon(hInstance, szAppName); 
    wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW); 
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); 
    wndclass.lpszMenuName  = szAppName; 
    wndclass.lpszClassName = szAppName; 
 
    if (!RegisterClass(&wndclass)) 
        return false; 
 
    // Create initial window frame.
    _hWnd = CreateWindow(szAppName, title, 
                         WS_CAPTION | WS_BORDER |  WS_SIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX ,
                         x,              // initial x position
                         y,              // initial y position
                         _width,         // initial x size
                         _height,        // initial y size
                         NULL, 
                         NULL, 
                         hInstance, 
                         NULL);

    if (!_hWnd)
        return false;

    SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);
    
    return true;
}

/// Show the window
///
/// @param nCmdShow [in] 
void window::show(int nCmdShow)
{
    ShowWindow(_hWnd, nCmdShow);
    UpdateWindow(_hWnd);
}

/// The window message loop
void window::messageLoop()
{
    MSG msg;

    while(GetMessage(&msg, _hWnd, 0, 0) > 0)
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
    }
}

/// Bring up a message box with the last error of a failing function
///
/// @param functionName [in] The name of the failing function
void window::messageBoxWithLastError(const char *functionName)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)functionName) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        functionName, dw, lpMsgBuf); 
    MessageBox(_hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

/// Bring up a message box showing an error
///
/// @param message [in] the error message
void window::messageBoxError(const char *message)
{
    MessageBox(_hWnd, message, "Error", MB_OK);
}
