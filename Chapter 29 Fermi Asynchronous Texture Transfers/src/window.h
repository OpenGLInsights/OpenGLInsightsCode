#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>

/// Encapsulate a window
class window
{
private:
    int _width;         ///< the current window width
    int _height;        ///< the current window height
    HWND _hWnd;         ///< window handle

    // this is a singleton, hide constructors etc.
    window();
    window(const window&);
    window& operator=(const window&);
    ~window();

public:
    /// Get an instance of this class
    ///
    /// @return instance
    static window &getInstance()
    {
        static window instance; 

        return instance; 
    }

    bool setup(HINSTANCE hInstance, int x, int y, const char *title);
    void show(int nCmdShow);
    void messageLoop();
    void messageBoxWithLastError(const char *functionName);
    void messageBoxError(const char *message);

    void resize(int newWidth, int newHeight);

    /// Get the window handle
    ///
    /// @return window handle
    HWND hWnd() const { return _hWnd; }
    /// Get the window width
    ///
    /// @return window width
    int width() const { return _width; }
    /// Get the window height
    ///
    /// @return window height
    int height() const { return _height; }
};

#endif // WINDOW_H