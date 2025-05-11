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

	glm::vec3 getPosition() const { return cameraPos; }
	glm::vec3 getFront() const { return cameraFront; }
	glm::vec3 getUp() const { return cameraUp; }

private:
	int width, height;
	glm::vec3 cameraPos, cameraFront, cameraUp;
	float theta, phi;
	float fov;
	glm::mat4 projection;
	glm::mat4 view;
};

#endif /* CAMERA_H */
