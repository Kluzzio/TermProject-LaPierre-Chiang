#include "window.h"
#include <iostream>

using namespace std;

Window::Window(const char* name, int* width, int* height)
{
    this->name = name;
    this->width = width;
    this->height = height;
    gWindow = NULL;
}

Window::~Window()
{
    if (gWindow)
    {
        glfwDestroyWindow(gWindow);
        gWindow = NULL;
    }
    glfwTerminate();
}

bool Window::Initialize()
{
    if (!glfwInit())
    {
        const char* err;
        glfwGetError(&err);
        cout << "GLFW failed to initialize: " << err << endl;
        return false;
    }

    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    gWindow = glfwCreateWindow(*width, *height, name, NULL, NULL);
    if (!gWindow)
    {
        cout << "GLFW window creation failed." << endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(gWindow);

    // GLEW init must come after context is current
    if (glewInit() != GLEW_OK)
    {
        cout << "GLEW failed to initialize." << endl;
        return false;
    }

    glfwSwapInterval(1);  // Enable VSync
    return true;
}

void Window::Swap()
{
    glfwSwapBuffers(gWindow);
}

GLFWwindow* Window::getWindow() const
{
    return gWindow;
}
