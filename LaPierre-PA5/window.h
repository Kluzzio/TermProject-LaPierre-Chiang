#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class Window
{
public:
    Window(const char* name, int* width, int* height);
    ~Window();

    bool Initialize();
    void Swap();
    GLFWwindow* getWindow() const;

private:
    GLFWwindow* gWindow;
    const char* name;
    int* width;
    int* height;
};

#endif /* WINDOW_H */
