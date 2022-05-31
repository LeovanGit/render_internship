#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <windows.h>
#include <windowsx.h>
#include <vector>

constexpr float quality = 2.0f;

class Window
{
public:
    Window() = default;

    void init(HINSTANCE hInstance,
              int pos_x, int pos_y,
              int client_width, int client_height);

    void resize(int width, int height);

    std::vector<int> & getPixels();

    RECT getSize() const;

    RECT getClientSize() const;

    SIZE getPixelsSize() const;

    void flush();

    HWND handle;
    HDC hdc;

    int pos_x;
    int pos_y;

private:    
    int client_width;
    int client_height;

    int pixels_width;
    int pixels_height;

    BITMAPINFO bmi;
    std::vector<int> pixels;
};

#endif
