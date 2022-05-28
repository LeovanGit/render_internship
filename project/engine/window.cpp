#include "window.hpp"

void Window::init(HINSTANCE hInstance,
                  int pos_x, int pos_y,
                  int client_width, int client_height)
{
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->client_width = client_width;
    this->client_height = client_height;

    // calculate the window size consided on client area
    RECT window = { 0, 0, client_width, client_height };
    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    handle = CreateWindowEx(0,
                            "window_class_1",
                            "raytracing",
                            WS_OVERLAPPEDWINDOW,
                            pos_x,
                            pos_y,
                            window.right - window.left,
                            window.bottom - window.top,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    hdc = GetDC(handle);

    // BITMAPINFO
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = client_width;
    bmi.bmiHeader.biHeight = -client_height; // inverted img
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits to represent pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    pixels.resize(client_width * client_height);
}

void Window::resize(int width, int height)
{
    client_width = width;
    client_height = height;

    pixels.resize(client_width * client_height);

    bmi.bmiHeader.biWidth = client_width;
    bmi.bmiHeader.biHeight = -client_height;
}

std::vector<int> & Window::getPixels() { return pixels; }

RECT Window::getSize() const
{
    RECT window;
    GetWindowRect(handle, &window);
    return window;
}

RECT Window::getClientSize() const
{
    RECT client;
    GetClientRect(handle, &client);
    return client;
}

void Window::flush()
{
    StretchDIBits(hdc,
                  0,
                  0,
                  client_width,
                  client_height,
                  0,
                  0,
                  client_width,
                  client_height,
                  pixels.data(),
                  &bmi,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}
