

#include "engine.h"
#include "glm/ext.hpp"

Engine::Engine(const char* name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;

}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  glfwSetCursorPosCallback(m_window->getWindow(), Engine::cursor_position_callback);
  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(m_window->getWindow(), scroll_callback);

  sensitivity = 10.f;
  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);


    // Update camera animation here.
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        m_graphics->getCamera()->Update(sensitivity/5, 0., 0., 0., 0., 0.);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        m_graphics->getCamera()->Update(-sensitivity/5, 0., 0., 0., 0., 0.);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        m_graphics->getCamera()->Update(0., sensitivity/5, 0., 0., 0., 0.);
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        m_graphics->getCamera()->Update(0., -sensitivity/5, 0., 0., 0., 0.);

    m_graphics->getCamera()->Update(0., 0., 0., sensitivity * (m_xpos - lastX) / m_WINDOW_WIDTH, 
                                                    sensitivity * (m_ypos - lastY) / m_WINDOW_HEIGHT, 0.);
    lastX = m_xpos;
    lastY = m_ypos;

    //Scroll Wheel
    m_graphics->getCamera()->Update(0., 0., 0., 0., 0., m_scroll);
    m_scroll = 0.f;
}

bool Engine::firstUpdate = true;
double Engine::m_xpos = 0.;
double Engine::m_ypos = 0.;
double Engine::lastX = 0.;
double Engine::lastY = 0.;
double Engine::m_scroll = 0.f;

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << "Position: (" << xpos << ":" << ypos << ")" << std::endl;
    Engine::m_xpos = xpos;
    Engine::m_ypos = ypos;
    if (firstUpdate) {
        lastX = xpos;
        lastY = ypos;
        firstUpdate = false;
    }
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    m_scroll -= yoffset;
}

unsigned int Engine::getDT()
{
  //long long TimeNowMillis = GetCurrentTimeMillis();
  //assert(TimeNowMillis >= m_currentTimeMillis);
  //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  //m_currentTimeMillis = TimeNowMillis;
  //return DeltaTimeMillis;
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
  //timeval t;
  //gettimeofday(&t, NULL);
  //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  //return ret;
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->Render();
    m_window->Swap();
    m_graphics->HierarchicalUpdate2(time);
}
