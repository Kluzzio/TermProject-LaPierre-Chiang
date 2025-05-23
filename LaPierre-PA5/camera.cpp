#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
	//--Init the view and projection matrices
	//  if you will be having a moving camera the view matrix will need to more dynamic
	//  ...Like you should update it before you render more dynamic 
	//  for this project having them static will be fine

	//view = glm::lookAt(glm::vec3(700., 700., 700.), //Eye Position
	//	glm::vec3(0.0, 0.0, 0.0), //Focus point
	//	glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

	cameraUp = { 0., 1., 0. };
	cameraPos = { 0., 700., -700. };
	theta = 90.f;
	phi = 135.f;
	fov = 45.f;
	width = w;
	height = h;
	Update(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
	return true;
}

void Camera::Update(float d_forward, float d_left, float d_up, float d_theta, float d_phi, float d_fov) {
	theta += d_theta;
	phi += d_phi;
	if (phi > 179.f) phi = 179.f;
	if (phi < 1.f) phi = 1.f;
	fov += d_fov;
	if (fov > 45.f) fov = 45.f;
	if (fov < 1.f) fov = 1.f;

	cameraFront = { cos(glm::radians(theta)) * sin(glm::radians(phi)),
					cos(glm::radians(phi)),
					sin(glm::radians(theta)) * sin(glm::radians(phi)) };
	cameraPos += d_forward * cameraFront;
	cameraPos += d_left * glm::normalize(glm::cross(cameraUp, cameraFront));

	view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
	//std::cout << theta << " " << phi << std::endl;

	projection = glm::perspective(glm::radians(fov), //the FoV typically 90 degrees is good which is what this is set to
		float(width) / float(height), //Aspect Ratio, so Circles stay Circular
		0.01f, //Distance to the near plane, normally a small value like this
		10000.0f); //Distance to the far plane, 
}


glm::mat4 Camera::GetProjection()
{
	return projection;
}

glm::mat4 Camera::GetView()
{
	return view;
}
