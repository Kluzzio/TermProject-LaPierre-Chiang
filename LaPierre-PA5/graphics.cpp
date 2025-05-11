#include "graphics.h"

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height)
{
	// Used for the linux OS
#if !defined(__APPLE__) && !defined(MACOSX)
  // cout << glewGetString(GLEW_VERSION) << endl;
	glewExperimental = GL_TRUE;

	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	//Check for error
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}
#endif



	// Init Camera
	m_camera = new Camera();
	if (!m_camera->Initialize(width, height))
	{
		printf("Camera Failed to Initialize\n");
		return false;
	}

	// Set up the shaders
	m_shader = new Shader();
	if (!m_shader->Initialize())
	{
		printf("Shader Failed to Initialize\n");
		return false;
	}

	// Add the vertex shader
	if (!m_shader->AddShader(GL_VERTEX_SHADER))
	{
		printf("Vertex Shader failed to Initialize\n");
		return false;
	}

	// Add the fragment shader
	if (!m_shader->AddShader(GL_FRAGMENT_SHADER))
	{
		printf("Fragment Shader failed to Initialize\n");
		return false;
	}

	// Connect the program
	if (!m_shader->Finalize())
	{
		printf("Program to Finalize\n");
		return false;
	}

	// Populate location bindings of the shader uniform/attribs
	if (!collectShPrLocs()) {
		printf("Some shader attribs not located!\n");
	}

	// Starship
	m_mesh = new Mesh(glm::vec3(2.0f, 3.0f, -5.0f), "assets\\SpaceShip-1.obj", "assets\\SpaceShip-1.png");

	glm::vec3 camPos = m_camera->getPosition();
	glm::vec3 camFront = glm::normalize(m_camera->getFront());
	glm::vec3 camUp = glm::normalize(m_camera->getUp());
	glm::vec3 camRight = glm::normalize(glm::cross(camFront, camUp));

	// Define offset in camera-relative directions: (Right, Up, Forward)
	glm::vec3 offset = (30.0f * camFront) + (-10.0f * camUp); // Behind and slightly below camera

	currentShipPos = camPos + offset;
	// Look in the same direction as the camera
	glm::vec3 direction = glm::normalize(-camFront);  // Forward direction
	glm::vec3 up = glm::normalize(camUp);            // World or camera up

	currentShipRot = glm::quatLookAt(direction, up);




	// Add celestial bodies with real orbital & rotational parameters
	// Scale note: 1 AU = 100 units; radius scale is arbitrary but relative
	CelestialBody* sun = new CelestialBody{
		"Sun", new Sphere(64, "assets\\2k_sun.jpg"), 
		"assets\\2k_sun.jpg", 0.0f, 0.0f, 7.25f, 609.12f,  // Rotation in hours (25.38 days), no orbit
		glm::vec3(50.f), nullptr, glm::radians(0.0f)
	};

	celestialBodies.push_back(*sun);

	// PLANETS
	celestialBodies.push_back({
		"Mercury", new Sphere(32, "assets\\mercury.jpg"),
		"assets\\mercury.jpg", 87.97f, 0.387f, 0.03f, 1407.6f,
		glm::vec3(5.f), sun, glm::radians(165.0f)
		});

	celestialBodies.push_back({
		"Venus", new Sphere(32, "assets\\venus.jpg"),
		"assets\\venus.jpg", 224.70f, 0.723f, 177.4f, -5832.5f,
		glm::vec3(12.f), sun, glm::radians(305.0f)
		});

	celestialBodies.push_back({
		"Earth", new Sphere(32, "assets\\2k_earth_daymap.jpg"),
		"assets\\2k_earth_daymap.jpg", 365.25f, 1.0f, 23.44f, 23.93f,
		glm::vec3(12.f), sun, glm::radians(75.0f)
		});

	celestialBodies.push_back({
		"Mars", new Sphere(32, "assets\\mars.jpg"),
		"assets\\mars.jpg", 686.98f, 1.524f, 25.19f, 24.6f,
		glm::vec3(10.f), sun, glm::radians(5.0f)
		});

	celestialBodies.push_back({
		"Jupiter", new Sphere(32, "assets\\jupiter.jpg"),
		"assets\\jupiter.jpg", 4332.59f, 5.203f, 3.13f, 9.9f,
		glm::vec3(30.f), sun, glm::radians(95.0f)
		});

	celestialBodies.push_back({
		"Saturn", new Sphere(32, "assets\\saturn.jpg"),
		"assets\\saturn.jpg", 10759.22f, 9.537f, 26.73f, 10.7f,
		glm::vec3(28.f), sun, glm::radians(265.0f)
		});

	celestialBodies.push_back({
		"Uranus", new Sphere(32, "assets\\uranus.jpg"),
		"assets\\uranus.jpg", 30688.5f, 19.191f, 97.77f, -17.2f,
		glm::vec3(24.f), sun, glm::radians(325.0f)
		});

	celestialBodies.push_back({
		"Neptune", new Sphere(32, "assets\\neptune.jpg"),
		"assets\\neptune.jpg", 60182.0f, 30.07f, 28.32f, 16.1f,
		glm::vec3(24.f), sun, glm::radians(15.0f)
		});

	// CERES (Dwarf planet in asteroid belt)
	celestialBodies.push_back({
		"Ceres", new Sphere(32, "assets\\ceres.jpg"),
		"assets\\ceres.jpg", 1680.0f, 2.77f, 4.0f, 9.07f,  // Orbit ~4.6 years, rotation ~9h
		glm::vec3(4.5f), sun, glm::radians(175.0f)
		});

	// ERIS (Scattered disc object, highly elliptical orbit)
	celestialBodies.push_back({
		"Eris", new Sphere(32, "assets\\eris.jpg"),
		"assets\\eris.jpg", 203600.0f, 67.67f, 44.0f, 25.9f,  // Orbit ~558 years, rotation ~26h
		glm::vec3(3.5f), sun, glm::radians(60.0f)
		});

	// HAUMEA (Elliptical dwarf planet, fast rotation, odd shape)
	celestialBodies.push_back({
		"Haumea", new Sphere(32, "assets\\haumea.jpg"),
		"assets\\haumea.jpg", 103774.0f, 43.13f, 28.2f, 3.9f,  // Orbit ~283 years, rotation ~4h
		glm::vec3(4.0f), sun, glm::radians(235.0f)
		});


	// EARTH'S MOON
	CelestialBody* earth = &celestialBodies[3]; // Index of Earth in the list
	celestialBodies.push_back({
		"Moon", new Sphere(32, "assets\\2k_moon.jpg"), 
		"assets\\2k_moon.jpg", 27.3f, 0.00257f, 6.68f, 655.7f, // Orbital period ~27.3 days, rotation ~27.3 days
		glm::vec3(0.27f), earth, glm::radians(75.0f)
		});


	// Add celestial bodies
	//celestialBodies = {
	//	{ "Mercury", nullptr, "assets\\mercury.jpg", opd, odau, atd, rph, scale, parent },
	//	{ "Mercury", nullptr, "assets\\mercury.jpg", {0.8f, 0.f, 0.8f}, {180.f, 0.f, 180.f}, 8.f, {0,1,0}, {8.f, 8.f, 8.f} },
	//	{ "Venus", nullptr, "assets\\venus.jpg", {0.6f, 0.f, 0.6f}, {250.f, 0.f, 250.f}, 6.f, {0,1,0}, {12.f, 12.f, 12.f} },
	//	{ "Mars", nullptr, "assets\\mars.jpg", {0.45f, 0.f, 0.45f}, {350.f, 0.f, 350.f}, 4.5f, {0,1,0}, {18.f, 18.f, 18.f} },
	//	{ "Jupiter", nullptr, "assets\\jupiter.jpg", {0.3f, 0.f, 0.3f}, {500.f, 0.f, 500.f}, 3.f, {0,1,0}, {60.f, 60.f, 60.f} },
	//	{ "Saturn", nullptr, "assets\\saturn.jpg", {0.25f, 0.f, 0.25f}, {600.f, 0.f, 600.f}, 2.5f, {0,1,0}, {55.f, 55.f, 55.f} },
	//	{ "Uranus", nullptr, "assets\\uranus.jpg", {0.2f, 0.f, 0.2f}, {700.f, 0.f, 700.f}, 2.f, {0,1,0}, {50.f, 50.f, 50.f} },
	//	{ "Neptune", nullptr, "assets\\neptune.jpg", {0.15f, 0.f, 0.15f}, {800.f, 0.f, 800.f}, 1.5f, {0,1,0}, {48.f, 48.f, 48.f} },
	//	{ "Ceres", nullptr, "assets\\ceres.jpg", {0.7f, 0.f, 0.7f}, {220.f, 0.f, 220.f}, 7.f, {0,1,0}, {5.f, 5.f, 5.f} },
	//	{ "Eris", nullptr, "assets\\eris.jpg", {0.05f, 0.f, 0.05f}, {1000.f, 0.f, 1000.f}, 0.5f, {0,1,0}, {15.f, 15.f, 15.f} },
	//	{ "Haumea", nullptr, "assets\\haumea.jpg", {0.06f, 0.f, 0.06f}, {950.f, 0.f, 950.f}, 0.6f, {0,1,0}, {13.f, 13.f, 13.f} }
	//};

	for (auto& body : celestialBodies) {
		body.object = new Sphere(48, body.texturePath.c_str());
	}



	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

void Graphics::HierarchicalUpdate2(double dt) {
	std::vector<float> speed, dist, scale;
	float rotSpeed;
	glm::vec3 rotVector;
	glm::mat4 tmat, smat, rmat;
	glm::mat4 localTransform;

	if (modelStack.empty()) {
		modelStack.push(glm::mat4(1.0f));  // Avoid empty stack error
	}

	for (auto& body : celestialBodies) {
		UpdateCelestialBodyTransform(body, dt);
	}

	// Clear the stack for safety
	while (!modelStack.empty()) modelStack.pop();

	if (modelStack.empty()) {
		modelStack.push(glm::mat4(1.0f));  // Push identity matrix
	}

	// Camera basis vectors
	glm::vec3 camPos = m_camera->getPosition();
	glm::vec3 camFront = glm::normalize(m_camera->getFront());
	glm::vec3 camUp = glm::normalize(m_camera->getUp());
	glm::vec3 camRight = glm::normalize(glm::cross(camFront, camUp));

	// Define offset behind and below camera
	glm::vec3 offset = (30.0f * camFront) + (-10.0f * camUp);
	glm::vec3 targetPos = camPos + offset;

	// --- Target Orientation (facing camera direction) ---
	glm::quat targetRot = currentShipRot;

	// Convert to radians
	float rollRad = glm::radians(roll);
	float pitchRad = glm::radians(pitch);
	float yawRad = glm::radians(yaw);

	// Build rotation relative to camera's local axes
	glm::quat q_pitch = glm::angleAxis(pitchRad, camRight);
	glm::quat q_yaw = glm::angleAxis(yawRad, camUp);
	glm::quat q_roll = glm::angleAxis(rollRad, camFront);

	// Apply in Yaw  Pitch  Roll order (common flight sim order)
	glm::quat userRot = q_yaw * q_pitch * q_roll;

	// Combine target facing + user input
	targetRot = targetRot * userRot;

	// Interpolation for smooth following
	float followSpeed = 5.0f;
	float t = glm::clamp(static_cast<float>(followSpeed * dt), 0.0f, 1.0f);

	// Interpolate position and rotation
	currentShipPos = glm::mix(currentShipPos, targetPos, t);
	currentShipRot = glm::slerp(currentShipRot, targetRot, t);

	// Final transforms
	tmat = glm::translate(glm::mat4(1.0f), currentShipPos);
	rmat = glm::mat4_cast(currentShipRot);
	smat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	localTransform = tmat * rmat * smat;
	if (m_mesh != NULL)
		m_mesh->Update(localTransform);


	while (!modelStack.empty()) modelStack.pop();

}

void Graphics::UpdateCelestialBodyTransform(CelestialBody& body, double dt) {
	glm::mat4 parentTransform = glm::mat4(1.0f);

	if (body.parent) {
		parentTransform = body.parent->object->GetModel(); // inherit parent transformation
	}
	glm::mat4 tmat = glm::mat4(1.0f);
	glm::mat4 rmat = glm::mat4(1.0f);
	glm::mat4 smat = glm::scale(glm::mat4(1.0f), body.scale);

	glm::mat4 parentTmat = glm::mat4(1.0f);

	if (body.parent) {
		float parentOrbitalSpeed = (2.0f * glm::pi<float>()) / (body.parent->orbitalPeriodDays * 24.0f * 3600.0f);
		float parentOrbitalAngle = fmod(body.parent->startingOrbitalPhaseRadians + parentOrbitalSpeed * (float)dt * 1000.0f, 2.0f * glm::pi<float>());
		float parentDistance = body.parent->orbitalDistanceAU * 150.0f;

		glm::mat4 parentOrbitRot = glm::rotate(glm::mat4(1.0f), parentOrbitalAngle, glm::vec3(0, 1, 0));
		glm::mat4 parentOrbitTrans = glm::translate(glm::mat4(1.0f), glm::vec3(parentDistance, 0, 0));
		parentTmat = parentOrbitRot * parentOrbitTrans;
	}

	// Orbital translation around parent (if not sun)
	if (body.parent) {
		float orbitalSpeed = (2.0f * glm::pi<float>()) / (body.orbitalPeriodDays * 24.0f * 3600.0f);
		float orbitalAngle = fmod(body.startingOrbitalPhaseRadians + orbitalSpeed * (float)dt * 1000.0f, 2.0f * glm::pi<float>());

		// If this is the moon, adjust its distance relative to the Earth, not the Sun
		float distance = body.name == "Moon" ? 0.00256f * 150.0f : body.orbitalDistanceAU * 150.0f;  // 0.00256 AU is the distance from Earth to Moon in AU

		glm::mat4 orbitRot = glm::rotate(glm::mat4(1.0f), orbitalAngle, glm::vec3(0, 1, 0));
		glm::mat4 orbitTrans = glm::translate(glm::mat4(1.0f), glm::vec3(distance, 0, 0));
		tmat = orbitRot * orbitTrans;
		if (body.parent->name != "Sun") {
			//tmat *= parentTmat;
		}

	}

	// Axial tilt (rotate on Z)
	glm::mat4 tiltMat = glm::rotate(glm::mat4(1.0f), glm::radians(body.axialTiltDegrees), glm::vec3(0, 0, 1));

	// Rotation around own axis (spin)
	float rotationAngle = static_cast<float>((2.0 * glm::pi<float>() * dt) / (body.rotationPeriodHours * 3600.0)) * 1000;  // 1000 seconds per second
	glm::mat4 spinMat = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0, 1, 0));

	rmat = tiltMat * spinMat;

	// Apply transformations (first parent's transformation, then its own)
	glm::mat4 localTransform = parentTransform * tmat * rmat * smat;

	body.object->Update(localTransform);

}

void Graphics::ComputeTransforms(double dt, std::vector<float> speed, std::vector<float> dist, 
	float rotSpeed, glm::vec3 rotVector, std::vector<float> scale, glm::mat4& tmat, glm::mat4& rmat, glm::mat4& smat) {
	tmat = glm::translate(glm::mat4(1.f),
		glm::vec3(cos(speed[0] * dt) * dist[0], sin(speed[1] * dt) * dist[1], sin(speed[2] * dt) * dist[2])
	);
	rmat = glm::rotate(glm::mat4(1.f), rotSpeed * (float)dt, rotVector);
	smat = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
}

void Graphics::Render()
{
	//clear the screen
	glClearColor(0.5, 0.2, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the correct program
	m_shader->Enable();

	// Send in the projection and view to the shader (stay the same while camera intrinsic(perspective) and extrinsic (view) parameters are the same
	glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection()));
	glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

	glUniform1i(m_sampler, 0);

	// Render the objects


	// Render Ship
	if (m_mesh != NULL) {
		glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mesh->GetModel()));
		m_mesh->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
	}

	for (auto& body : celestialBodies) {
		if (body.object != nullptr) {
			glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(body.object->GetModel()));
			body.object->Render(m_positionAttrib, m_colorAttrib, m_tcAttrib, m_hasTexture);
		}
	}


	// Get any errors from OpenGL
	auto error = glGetError();
	if (error != GL_NO_ERROR)
	{
		string val = ErrorString(error);
		std::cout << "Error initializing OpenGL! " << error << ", " << val << std::endl;
	}
}


bool Graphics::collectShPrLocs() {
	bool anyProblem = true;
	// Locate the projection matrix in the shader
	m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
	if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_projectionMatrix not found\n");
		anyProblem = false;
	}

	// Locate the view matrix in the shader
	m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
	if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_viewMatrix not found\n");
		anyProblem = false;
	}

	// Locate the model matrix in the shader
	m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
	if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
	{
		printf("m_modelMatrix not found\n");
		anyProblem = false;
	}

	// Locate the position vertex attribute
	m_positionAttrib = m_shader->GetAttribLocation("v_position");
	if (m_positionAttrib == -1)
	{
		printf("v_position attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_colorAttrib = m_shader->GetAttribLocation("v_color");
	if (m_colorAttrib == -1)
	{
		printf("v_color attribute not found\n");
		anyProblem = false;
	}

	// Locate the color vertex attribute
	m_tcAttrib = m_shader->GetAttribLocation("v_tc");
	if (m_tcAttrib == -1)
	{
		printf("v_texcoord attribute not found\n");
		anyProblem = false;
	}

	m_hasTexture = m_shader->GetUniformLocation("hasTexture");
	if (m_hasTexture == INVALID_UNIFORM_LOCATION) {
		printf("hasTexture uniform not found\n");
		anyProblem = false;
	}
	m_sampler = m_shader->GetUniformLocation("sp");
	if (m_sampler == INVALID_UNIFORM_LOCATION)
	{
		printf("Sampler Not found not found\n");
		anyProblem = false;
	}

	return anyProblem;
}

std::string Graphics::ErrorString(GLenum error)
{
	if (error == GL_INVALID_ENUM)
	{
		return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
	}

	else if (error == GL_INVALID_VALUE)
	{
		return "GL_INVALID_VALUE: A numeric argument is out of range.";
	}

	else if (error == GL_INVALID_OPERATION)
	{
		return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
	}

	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
		return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
	}

	else if (error == GL_OUT_OF_MEMORY)
	{
		return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
	}
	else
	{
		return "None";
	}
}

