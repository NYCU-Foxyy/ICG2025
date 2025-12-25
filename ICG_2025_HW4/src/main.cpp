#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "header/stb_image.h"
#include "header/Object.h"
#include "header/Shader.h"
#include "header/Material.h"
#include "header/Light.h"
#include "header/Camera.h"

// enable error debugging
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
				 GLuint id,
				 GLenum severity,
				 GLsizei length,
				 const GLchar* message,
				 const void* userParam )
{
	fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	         ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
			  type, severity, message );
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
glm::vec4 backgroundColor(0.0f, 0.0f, 0.0f, 1.0f);

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct TimeInfo {
	float currentTime;
	float deltaTime;

	TimeInfo(): currentTime(glfwGetTime()), deltaTime(0.0f) {}
	
	void update() {
		float cur = glfwGetTime();
		deltaTime = cur - currentTime;
		currentTime = cur;
	}
};

GLFWwindow* setupWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW4", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSwapInterval(1);

	if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	return window;
}

/***** Global Objects *****/

Light light(glm::vec3(1000.0, 1000.0, 0.0), glm::vec3(1.0), glm::vec3(1.0), glm::vec3(1.0));
Camera camera(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
Object* sphereObj;
Object* diskObj;

glm::mat4 sphereModel(1.0f);
glm::mat4 diskModel(1.0f);

vector<Shader*> shaders;
int shaderGlobalIndex = 0;

/***** Global Objects *****/

/***** Animation Parameters *****/

float sphereRadius = 1.0;
float sphereSpeed = 0.0;

float spikeHeight = 0.25;
bool spikeActive = false;

float gravity = 1.0;

/***** Animation Parameters *****/

void setupObjects() {
	sphereObj = new Object("../../src/asset/obj/sphere.obj");
	diskObj = new Object("../../src/asset/obj/disk.obj");
	
	sphereObj->loadTexture("../../src/asset/texture/wood_0060_color_1k.jpg");
	diskObj->loadTexture("../../src/asset/texture/disk_color_1k.png");

	sphereObj->setMaterial(Material(glm::vec3(0.5f), glm::vec3(1.0f), glm::vec3(1.0f)));

	sphereModel = glm::translate(sphereModel, glm::vec3(0.0f, 4.0f, 0.0f));
}

void setupGL() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
//	glCullFace(GL_BACK);
	
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}

void setupShaders() {
	std::string shaderDir = "../../src/shaders/";

	std::vector<std::array<std::string, 3>> shaderNames{
		{{"bling-phong", "default", "bling-phong"}},
		{{"bling-phong", "spike", "bling-phong"}}
	};

	for (auto names : shaderNames) {
		std::string vpath =	shaderDir + names[0] + ".vert";
		std::string gpath = shaderDir + names[1] + ".geom";
		std::string fpath = shaderDir + names[2] + ".frag";
		
		Shader* shader = new Shader();
		shader->create();
		shader->add_shader(vpath, GL_VERTEX_SHADER);
		shader->add_shader(gpath, GL_GEOMETRY_SHADER);
		shader->add_shader(fpath, GL_FRAGMENT_SHADER);
		shader->link_shader();
		shaders.push_back(shader);
	}
}

float getPointToPlaneDistance(glm::vec3 p, glm::vec3 normal, glm::vec3 planePoint) {
	float t = glm::dot(normal, planePoint - p) / glm::dot(normal, normal);
	return t;
}

float sign(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool isPointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

void renderModel(int shaderId, glm::mat4 model, Object* obj) {
	glm::mat4 view = glm::lookAt(camera.getPosition(), camera.getTarget(), camera.getUp());
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	
	auto& shader = shaders[shaderId];
	shader->use();
	shader->set_uniform_value("view", view);
	shader->set_uniform_value("projection", projection);
	
	shader->set_uniform_value("light.position", light.position);
	shader->set_uniform_value("light.ambient", light.ambient);
	shader->set_uniform_value("light.diffuse", light.diffuse);
	shader->set_uniform_value("light.specular", light.specular);

	shader->set_uniform_value("material.ambient", sphereObj->getMaterial().ambient);
	shader->set_uniform_value("material.diffuse", sphereObj->getMaterial().diffuse);
	shader->set_uniform_value("material.specular", sphereObj->getMaterial().specular);

	shader->set_uniform_value("spikeHeight", spikeHeight);

	shader->set_uniform_value("model", model);

	obj->draw();

	shader->release();
}

void render() {
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderModel(shaderGlobalIndex, sphereModel, sphereObj);
	renderModel(0, diskModel, diskObj);
}

void cleanup() {
	delete sphereObj;
	delete diskObj;

	for (auto shader : shaders)
		delete shader;
}

void update(TimeInfo& timeInfo) {
	glm::vec3 spherePosition = glm::vec3(sphereModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	bool collide = false;
	glm::vec3 t1, t2, t3;
	for (int poly_i = 0; poly_i * 9 < diskObj->positions.size(); poly_i++) {
		glm::vec3 p1(diskObj->positions[poly_i * 9 + 0], diskObj->positions[poly_i * 9 + 1], diskObj->positions[poly_i * 9 + 2]);
		glm::vec3 p2(diskObj->positions[poly_i * 9 + 3], diskObj->positions[poly_i * 9 + 4], diskObj->positions[poly_i * 9 + 5]);
		glm::vec3 p3(diskObj->positions[poly_i * 9 + 6], diskObj->positions[poly_i * 9 + 7], diskObj->positions[poly_i * 9 + 8]);
		glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
		float dist = abs(getPointToPlaneDistance(spherePosition, normal, p1));
		glm::vec3 projectionPoint = spherePosition + dist * normal;
		float distThreshold = sphereRadius;
		if (spikeActive)
			distThreshold += spikeHeight;
		if (dist < distThreshold and isPointInTriangle(projectionPoint, p1, p2, p3)) {
			collide = true;
			t1 = p1, t2 = p2, t3 = p3;
		}
	}
	if (not collide) {
		sphereSpeed += -gravity * timeInfo.deltaTime;
	} else {
		sphereSpeed = 0;
	}
	sphereModel = glm::translate(sphereModel, glm::vec3(0.0f, sphereSpeed * timeInfo.deltaTime, 0.0f));
}

int main(void) {
	glfwInit();

	GLFWwindow* window = setupWindow();
	
	setupObjects();
	setupGL();
	setupShaders();

	TimeInfo timeInfo;
	while (not glfwWindowShouldClose(window)) {
//		processInput(window);
		timeInfo.update();
		update(timeInfo);
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanup();

	glfwTerminate();
	return 0;
}

/*
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
*/

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE and action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_E and action == GLFW_PRESS)
		shaderGlobalIndex = glm::clamp(shaderGlobalIndex + 1, 0, (int)shaders.size() - 1);
	if (key == GLFW_KEY_Q and action == GLFW_PRESS)
		shaderGlobalIndex = glm::clamp(shaderGlobalIndex - 1, 0, (int)shaders.size() - 1);
	
	spikeActive = (shaderGlobalIndex == 1);

	if (key == GLFW_KEY_SPACE and action == GLFW_PRESS) {
		sphereModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}
