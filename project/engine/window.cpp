#include "window.hpp"

namespace
{
constexpr float quality = 1.0f;
} // namespace

void Window::init(HINSTANCE hInstance,
                  int pos_x, int pos_y,
                  int client_width, int client_height)
{
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->client_width = client_width;
    this->client_height = client_height;
    this->pixels_width = client_width / quality;
    this->pixels_height = client_height / quality;

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
    bmi.bmiHeader.biWidth = pixels_width;
    bmi.bmiHeader.biHeight = -pixels_height; // inverted img
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits to represent pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    pixels.resize(pixels_width * pixels_height);
}

void Window::resize(int width, int height)
{
    client_width = width;
    client_height = height;

    this->pixels_width = width / quality;
    this->pixels_height = height / quality;

    pixels.resize(pixels_width * pixels_height);

    bmi.bmiHeader.biWidth = pixels_width;
    bmi.bmiHeader.biHeight = -pixels_height;
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

SIZE Window::getPixelsSize() const
{
    SIZE pixels_size;
    pixels_size.cx = pixels_width;
    pixels_size.cy = pixels_height;
    return pixels_size;
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
                  pixels_width,
                  pixels_height,
                  pixels.data(),
                  &bmi,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}
