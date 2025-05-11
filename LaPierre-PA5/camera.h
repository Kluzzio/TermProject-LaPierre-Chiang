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

private:
	int width, height;
	glm::vec3 cameraPos, cameraFront, cameraUp;
	float theta, phi;
	float fov;
	glm::mat4 projection;
	glm::mat4 view;
};

#endif /* CAMERA_H */
