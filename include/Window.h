// Window class for Raylib game
#ifndef WINDOW_H
#define WINDOW_H
class Window {
    private:
        int screenWidth;
        int screenHeight;
        const char* title;
    public:
        Window();
        Window(int screenWidth, int screenHeight, const char* title);
        void init();
        void setTargetFPS(int fps);
        void beginDrawing();
        void endDrawing();
        bool shouldClose();
        void close();
        int getScreenWidth();
        int getScreenHeight();
        void setWindowPos(int x, int y);
        const char* getTitle();
        void positionWindowOnSecondMonitor();
};
#endif
