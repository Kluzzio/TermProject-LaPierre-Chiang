#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
public:
	Camera();
	~Camera();
	bool Initialize(int w, int h);
	glm::mat4 GetProjection();
	glm::mat4 GetView();
	void Update(float d_forward, float d_left, float d_up, float d_theta, float d_phi, float d_fov);
	void Update(float d_forward, float d_left, float d_up, float d_theta, float d_phi, float d_fov, bool freefly);

	void setPosition(glm::vec3 pos) { cameraPos = pos; }
	void setLookAt(glm::mat4 lookAt) { view = lookAt; }
	void setFront(glm::vec3 front) { cameraFront = front; }
	void setUp(glm::vec3 up) { cameraUp = up; }

	glm::vec3 getPosition() const { return cameraPos; }
	glm::vec3 getFront() const { return cameraFront; }
	glm::vec3 getUp() const { return cameraUp; }

	void setTheta(float t) { theta = t; }
	void setPhi(float p) { phi = p; }

private:
	int width, height;
	glm::vec3 cameraPos, cameraFront, cameraUp;
	float theta, phi, roll;
	float fov;
	glm::mat4 projection;
	glm::mat4 view;
};

#endif /* CAMERA_H */
