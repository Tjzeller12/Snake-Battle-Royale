#include "Window.h"
#include "raylib.h"
// Default constructor
Window::Window()
{
    screenWidth = 800;
    screenHeight = 450;
    title = "Snake Battle Royale";
}
// Overloaded constructor
Window::Window(int screenWidth, int screenHeight, const char *title)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->title = title;
}
// Initializes window
void Window::init()
{
    InitWindow(screenWidth, screenHeight, title);
}
// Target FPS setter
void Window::setTargetFPS(int fps)
{
    SetTargetFPS(fps);
}
// Calls begin drawing function
void Window::beginDrawing()
{
    BeginDrawing();
}
// Calls end drawing function
void Window::endDrawing()
{
    EndDrawing();
}
// Window should close getter
bool Window::shouldClose()
{
    return WindowShouldClose();
}
// Calls close window
void Window::close()
{
    CloseWindow();
}
// Screen width getter
int Window::getScreenWidth()
{
    return screenWidth;
}
// screen height getter
int Window::getScreenHeight()
{
    return screenHeight;
}
// title getter
const char *Window::getTitle()
{
    return title;
}
// Window position setter
void Window::setWindowPos(int x, int y)
{
    SetWindowPosition(x, y);
}
// Puts the position of the window on second monitor if there is a second
// monitor
void Window::positionWindowOnSecondMonitor()
{
    int monitor_count = GetMonitorCount();
    if (monitor_count > 1)
    {
        int primary_monitor_w = GetMonitorWidth(0);
        int secondary_monitor_w = GetMonitorWidth(1);

        Vector2 second_monitor_pos = GetMonitorPosition(1);
        int pos_x = second_monitor_pos.x + 50;
        int pos_y = second_monitor_pos.y + 50;
        this->setWindowPos(pos_x, pos_y);
    }
    else
    {
        this->setWindowPos(50, 50);
    }
}