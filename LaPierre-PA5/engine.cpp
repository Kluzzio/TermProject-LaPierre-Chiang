

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
  speed = 0.f;
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

    float yawInput = 0.0f;
    float pitchInput = 0.0f;
    float rollInput = 0.0f;

    bool isMKeyPressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_M) == GLFW_PRESS;
    if (isMKeyPressed && !wasMKeyPressed) {
        m_graphics->setGamemode(!m_graphics->getGamemode());

        Camera* m_camera = m_graphics->getCamera();
        glm::vec3 camPos = m_camera->getPosition();
        Sphere* closestSphere = m_graphics->GetClosestSpherePosition(camPos);
        glm::vec3 closestSpherePos = closestSphere->GetModel()[3];

        savedClosestSpherePos = closestSpherePos;
        savedClosestSphere = closestSphere;

        //std::cout << "Closest sphere position: "
        //    << glm::to_string(closestSpherePos) << std::endl;

        // Gamemode: Planet observation
        if (!m_graphics->getGamemode()) {
            oldCamPos = camPos;
            oldFront = m_camera->getFront();

            // === Move camera to a fixed distance away from the sphere ===
            const float desiredDistance = 85.0f;  // Set your desired distance here

            glm::vec3 direction = glm::normalize(glm::vec3(0, 0, 0) - closestSpherePos); // From sphere toward origin
            glm::vec3 worldUp = glm::vec3(0, 1, 0);
            glm::vec3 leftDirection = glm::normalize(glm::cross(worldUp, direction));
            glm::vec3 newCamPos = closestSpherePos + leftDirection * desiredDistance;

            // === Update camera position and orientation ===
            m_camera->setLookAt(glm::lookAt(newCamPos, closestSpherePos, m_camera->getUp()));

            glm::vec3 front = glm::normalize(closestSpherePos - newCamPos);
            float newTheta = glm::degrees(atan2(front.z, front.x));
            float newPhi = glm::degrees(acos(front.y)); // since front is normalized

            m_camera->setPosition(newCamPos);
            m_camera->setFront(front);

            // Now sync the angles so Update() maintains this direction
            m_camera->setTheta(newTheta);
            m_camera->setPhi(newPhi);
        }
        // Gamemode: Exploration
        else {

            m_camera->setPosition(oldCamPos);
            m_camera->setFront(oldFront);

            float newTheta = glm::degrees(atan2(oldFront.z, oldFront.x));
            float newPhi = glm::degrees(acos(oldFront.y)); // since front is normalized
            m_camera->setTheta(newTheta);
            m_camera->setPhi(newPhi);
        }

        speed = 0.0;
    }
    // Planet Observation Reset Button
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_R) == GLFW_PRESS && !m_graphics->getGamemode()) {
        Camera* m_camera = m_graphics->getCamera();
        glm::vec3 camPos = m_camera->getPosition();

        float scaleFactor = m_graphics->GetScaleFromSphere(savedClosestSphere).x; // or .y or .z — they're the same
        const float desiredDistance = 20.0f * (scaleFactor / 15.0f) + 40.0f;

        glm::vec3 closestSpherePos = savedClosestSphere->GetModel()[3];
        
        // === Move camera to a fixed distance away from the sphere ===

        glm::vec3 direction = glm::normalize(glm::vec3(0, 0, 0) - closestSpherePos); // From sphere toward origin
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        glm::vec3 leftDirection = glm::normalize(glm::cross(worldUp, direction));
        glm::vec3 newCamPos = closestSpherePos + leftDirection * desiredDistance;

        // === Update camera position and orientation ===
        m_camera->setLookAt(glm::lookAt(newCamPos, closestSpherePos, m_camera->getUp()));

        glm::vec3 front = glm::normalize(closestSpherePos - newCamPos);
        float newTheta = glm::degrees(atan2(front.z, front.x));
        float newPhi = glm::degrees(acos(front.y)); // since front is normalized

        m_camera->setPosition(newCamPos);
        m_camera->setFront(front);

        // Now sync the angles so Update() maintains this direction
        m_camera->setTheta(newTheta);
        m_camera->setPhi(newPhi);
    }
    // Gamemode: Exploration
    if (m_graphics->getGamemode()) {

        //Only have these controls if we have our starship

        // Update camera animation here.
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            speed += .2;
            speed = glm::clamp(0.0, speed, 10.0);
        }
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            speed -= .2;
            speed = glm::clamp(0.0, speed, 10.0);
        }
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., (sensitivity + 3.0 * speed) / 20, 0., 0., 0., 0.);
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., -(sensitivity + 3.0 * speed) / 20, 0., 0., 0., 0.);
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
            yawInput = 0.6f; // Yaw left
        }
        else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
            yawInput = -0.6f; // Yaw right
        }
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
            pitchInput = 0.4f; // Pitch up
        }
        else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
            pitchInput = -0.4f; // Pitch down
        }

        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
            rollInput = -2.0f;  // Roll left
        }
        else if (glfwGetKey(m_window->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
            rollInput = 2.0f;   // Roll right
        }
        m_graphics->getCamera()->Update(speed / 5, 0., 0., 0., 0., 0.);

        if (rollInput != 0.0f || yawInput != 0.0f || pitchInput != 0.0f) {
            Camera* m_camera = m_graphics->getCamera();
            glm::vec3 camPos = m_camera->getPosition();

            // Face same direction as starship
            glm::vec3 forward = m_graphics->getCurrentShipRot() * glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 up = m_graphics->getCurrentShipRot() * glm::vec3(0.0f, 1.0f, 0.0f);


            glm::vec3 camTarget = m_graphics->getCurrentShipPos() + forward;
            m_camera->setFront(glm::normalize(camTarget - camPos));

            float newTheta = glm::degrees(atan2(forward.z, forward.x)) - yawInput;
            float newPhi = glm::degrees(acos(forward.y)) - pitchInput;
            m_camera->setTheta(newTheta); //yaw
            m_camera->setPhi(newPhi); //pitch

            float rollAngle = glm::degrees(atan2(
                glm::dot(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), up), forward),
                glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), up)
            ));
            m_camera->setRoll(rollAngle - rollInput); // Or add/subtract depending on input convention


        }

        m_graphics->UpdateRotation(yawInput, pitchInput, rollInput);
    }
    // Gamemode: Planetary Observation
    else {
        // Update camera animation here.
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., 0., (sensitivity) / 20, 0., 0., 0., true);
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., 0., -(sensitivity) / 20, 0., 0., 0., true);
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., (sensitivity) / 20, 0., 0., 0., 0.);
        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            m_graphics->getCamera()->Update(0., -(sensitivity) / 20, 0., 0., 0., 0.);

        bool moving =
            glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS ||
            glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS ||
            glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS ||
            glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS;

        Camera* m_camera = m_graphics->getCamera();
        glm::vec3 camPos = m_camera->getPosition();

        glm::vec3 closestSpherePos = glm::vec3(savedClosestSphere->GetModel()[3]);

        // Compute direction from sphere to current camera position (preserving relative direction)
        glm::vec3 sphereToCamDir = glm::normalize(camPos - closestSpherePos);

        // Use scale to calculate appropriate distance
        float scaleFactor = m_graphics->GetScaleFromSphere(savedClosestSphere).x; // assuming uniform scale
        const float desiredDistance = 20.0f * (scaleFactor / 15.0f) + 40.0f;

        // Compute new camera position along preserved direction
        glm::vec3 newCamPos = closestSpherePos + sphereToCamDir * desiredDistance;

        // Update camera
        if (moving) {
            glm::vec3 front = glm::normalize(closestSpherePos - newCamPos); // camera should still look at the sphere
            float newTheta = glm::degrees(atan2(front.z, front.x));
            float newPhi = glm::degrees(acos(front.y)); // acos assumes front is normalized
            m_camera->setFront(front);
            m_camera->setTheta(newTheta);
            m_camera->setPhi(newPhi);
        }

        m_camera->setPosition(newCamPos);

        // Optional: update the lookAt matrix if your camera system uses it internally
        m_camera->setLookAt(glm::lookAt(newCamPos, closestSpherePos, m_camera->getUp()));
    }

    m_graphics->getCamera()->Update(0., 0., 0., sensitivity * (m_xpos - lastX) / m_WINDOW_WIDTH,
        sensitivity * (m_ypos - lastY) / m_WINDOW_HEIGHT, 0.);
    lastX = m_xpos;
    lastY = m_ypos;

    //Scroll Wheel
    m_graphics->getCamera()->Update(0., 0., 0., 0., 0., m_scroll);
    m_scroll = 0.f;
    wasMKeyPressed = isMKeyPressed;
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
