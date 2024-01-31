#include "Window.h"
#include "raylib.h"
Window::Window() {
    screenWidth = 800;
    screenHeight = 450;
    title = "Snake Battle Royale";
}
Window::Window(int screenWidth, int screenHeight, const char* title) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->title = title;

}
void Window::init() {
    InitWindow(screenWidth, screenHeight, title);
}
void Window::setTargetFPS(int fps) {
    SetTargetFPS(fps);
}
void Window::beginDrawing() {
    BeginDrawing();
}
void Window::endDrawing() {
    EndDrawing();
}
bool Window::shouldClose() {
    return WindowShouldClose();
}
void Window::close() {
    CloseWindow();
}
int Window::getScreenWidth() {
    return screenWidth;
}
int Window::getScreenHeight() {
    return screenHeight;
}
const char* Window::getTitle() {
    return title;
}
void Window::setWindowPos(int x, int y) {
    SetWindowPosition(x, y);
}
void Window::positionWindowOnSecondMonitor() {
    int monitor_count = GetMonitorCount();
    if(monitor_count > 1) {
        int primary_monitor_w = GetMonitorWidth(0);
        int secondary_monitor_w = GetMonitorWidth(1);

        Vector2 second_monitor_pos = GetMonitorPosition(1);
        int pos_x = second_monitor_pos.x + 50;
        int pos_y = second_monitor_pos.y + 50;
        this->setWindowPos(pos_x, pos_y);
    } else {
        this->setWindowPos(50, 50);
    }
}