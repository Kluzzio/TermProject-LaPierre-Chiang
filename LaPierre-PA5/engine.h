#ifndef ENGINE_H
#define ENGINE_H


#include <assert.h>
#include "window.h"
#include "graphics.h"


class Engine
{
  public:
    Engine(const char*  name, int width, int height);
    
    ~Engine();
    bool Initialize();
    void Run();
    void ProcessInput();
    unsigned int getDT();
    long long GetCurrentTimeMillis();
    void Display(GLFWwindow*, double);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

  
  private:
    // Window related variables
    Window *m_window;    
    const char* m_WINDOW_NAME;
    int m_WINDOW_WIDTH;
    int m_WINDOW_HEIGHT;
    bool m_FULLSCREEN;


    Graphics *m_graphics;

    bool m_running;

    static double m_xpos, m_ypos, m_scroll;
    static double lastX, lastY;
    double sensitivity;
    static bool firstUpdate;
    double speed;
};

#endif // ENGINE_H
