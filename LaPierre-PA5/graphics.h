#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sphere.h"
#include "mesh.h"

#define numVBOs 2;
#define numIBs 2;

struct CelestialBody {
    std::string name;
    Sphere* object;
    std::string texturePath;

    float orbitalPeriodDays;     // days to orbit parent
    float orbitalDistanceAU;     // distance from parent in AU (1 AU = 149.6 million km)
    float axialTiltDegrees;      // axial tilt in degrees
    float rotationPeriodHours;   // rotation period in Earth hours

    glm::vec3 scale;             // size of the object
    CelestialBody* parent;       // pointer to parent body (nullptr for Sun)
    float startingOrbitalPhaseRadians;
};


class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void HierarchicalUpdate2(double dt);
    void UpdateCelestialBodyTransform(CelestialBody& body, double dt);
    void Render();

    Camera* getCamera() { return m_camera; }

    void UpdateRotation(float yawInput, float pitchInput, float rollInput) {
        yaw = yawInput;
        pitch = pitchInput;
        roll = rollInput;
    }

  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs();
    void ComputeTransforms (double dt, std::vector<float> speed, std::vector<float> dist,
        float rotSpeed, glm::vec3 rotVector, std::vector<float> scale, 
        glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat);

    stack<glm::mat4> modelStack;

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
    GLint m_positionAttrib;
    GLint m_colorAttrib;
    GLint m_tcAttrib;
    GLuint m_sampler;
    GLint m_hasTexture;

    std::vector<CelestialBody> celestialBodies;

    Sphere* m_sphere;
    Sphere* m_sphere2;
    Sphere* m_sphere3;
    Sphere* m_ceres;
    Sphere* m_eris;
    Sphere* m_haumea;
    Sphere* m_jupiter;
    Sphere* m_mars;
    Sphere* m_mercury;
    Sphere* m_neptune;
    Sphere* m_saturn;
    Sphere* m_uranus;
    Sphere* m_venus;


    Mesh* m_mesh;

    glm::vec3 currentShipPos;
    glm::quat currentShipRot;

    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
};

#endif /* GRAPHICS_H */
