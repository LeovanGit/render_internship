#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // REGISTER THE WINDOW CLASS
    // handle of the window
    HWND hWnd;
    // store information for the window class
    WNDCLASSEX wc;

    // clear the window class:
    // init memory block (which starts from &wc with given size) by NULL
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // window class properties
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass1";

    RegisterClassEx(&wc);

    // calculate the window size consided on client area
    RECT wr = {0, 0, 500, 400};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // CREATE THE WINDOW (using window class above)
    hWnd = CreateWindowEx(NULL,
                          "WindowClass1",
                          "Window",
                          WS_OVERLAPPEDWINDOW,
                          300,    // x-pos
                          300,    // y-pos
                          wr.right - wr.left,    // width
                          wr.bottom - wr.top,    // height
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    // MAIN LOOP (EVENT HANDLING)
    // stores messages from Windows
    MSG msg;

    while(true)
    {
        // take another msg from the event queue
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate key-pressed messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);

            // quit
            if(msg.message == WM_QUIT) break;
        }
        else
        {
            // game code
        }
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        // window closed
        case WM_DESTROY:
            {
                // close the app => send WM_QUIT msg
                PostQuitMessage(0); // 0 is WM_QUIT
                return 0;
            } break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
