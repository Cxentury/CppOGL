#include "headers/scene.h"
#include "headers/light.h"
#include "headers/model.h"
#include "headers/logger.h"
#include "headers/pointLight.h"
#include "headers/directionalLight.h"
#include "headers/postProcessing.h"
#include <iostream>
#include <iterator>
#include <filesystem>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

float deltaTime = 0;
bool firstMouse = true;
float lastX, lastY;

Camera camera;
bool camera_control = false;

uint16_t Scene::width = 800;
uint16_t Scene::height = 600;

Scene::Scene(uint16_t width, uint16_t height) {
	Scene::width = width;
	Scene::height = height;

	this->m_pWindow = this->initWindow();
	std::vector<DefaultGui*> list = {
		new UserParameters(true)
	};
	this->m_gui = DefaultGui(true, list);
}

Scene::~Scene() {
	this->m_gui.closeLibrary();
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

GLFWwindow* Scene::initWindow() {
	Scene::initGLFW();
	GLFWwindow* window = Scene::createWindow();
	Scene::initGLAD();

	return window;
}

void Scene::initGLFW() {
	glfwInit();

	// Define OpenGl version to use
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* Scene::createWindow() {

	GLFWwindow* window = glfwCreateWindow(Scene::width, Scene::height, "3D engine", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Define the created window as the main context
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	return window;
}

void Scene::initGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		exit(EXIT_FAILURE);
	}
}

void Scene::setOpenGLOptions() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::setCallBacks() {
	glfwSetCursorPosCallback(this->m_pWindow, mouse_callback);
	glfwSetInputMode(this->m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(this->m_pWindow, key_callback);

}
void Scene::renderLoop() {


	//Adds all lights, models, framebuffers to the scene
	setupScene();
	setCallBacks();
	setOpenGLOptions();
	
	Model sceneModel{ "models/postProcessing/quad.obj" };

	//Keep if either of the stencil or depth test fails
	//and replace if both succeed
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	float lastFrame = 0.0f, current = 0.0f;
	int frame = 0;

	this->m_gui.init(m_pWindow);

	while (!glfwWindowShouldClose(m_pWindow)) {


		current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

		//dont put this-> it will make the line too long
		//draw normal scene
		Framebuffer* fb = m_framebuffers.find("framebuffer")->second;
		fb->use(Scene::width, Scene::height);
		fb->setOuputTexture(0);
		this->drawScene("default");

		if (pProcessing.getBool("blur")) {
			//draw the blured scene
			Shader* blur = m_shaders.find("boxBlur")->second;
			fb->setInputTextures(*blur, 1);
			fb->setOuputTexture(1);
			sceneModel.draw(*blur);
		}
		else if(pProcessing.getBool("bokeh")){
			Shader* bokeh = m_shaders.find("bokeh")->second;
			fb->setInputTextures(*bokeh, 1);
			fb->setOuputTexture(1);
			sceneModel.draw(*bokeh);
		}
		Shader* pProcessingShader = m_shaders.find("postProcessing")->second;
		pProcessing.updateUniforms(*pProcessingShader);

		//Default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		fb->setInputTextures(*pProcessingShader, 2);
		sceneModel.draw(*pProcessingShader);

		this->m_gui.render(this);

		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}
}

void Scene::drawScene(std::string shaderName) {

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	camera.processInput(this->m_pWindow, deltaTime);
	camera.update();

	glm::mat4 projection;					   //FOV	         //Aspect ratio                              //near //far plane frustum
	projection = glm::perspective(glm::radians(camera.getFov()), (float)Scene::width / (float)Scene::height, 0.1f, 100.0f);

	//Prevent writing to the stencil buffer
	glStencilMask(0x00);
	m_cubemaps.find("ocean")->second->draw(camera.getLookAtMatrix(), projection);

	//Must use the shader before calling glUniform()
	Shader* shader = m_shaders.find(shaderName)->second;
	shader->use();

	//Model
	shader->setVec3("viewPos", camera.getPos());
	shader->setMatrix4("view", camera.getLookAtMatrix());
	shader->setMatrix4("projection", projection);

	Shader* outlineShader = m_shaders.find("outline")->second;
	outlineShader->use();
	outlineShader->setMatrix4("view", camera.getLookAtMatrix());
	outlineShader->setMatrix4("projection", projection);


	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (Model* model : this->getModels()) {
		if (model->getActive())
			model->draw(*shader);
	}

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	for (Model* upScaledModel : this->getModels()) {
		if (upScaledModel->isOutlined())
			upScaledModel->draw(*outlineShader);
	}
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);


	//Lights
	Shader* lightShader = m_shaders.find("light")->second;
	lightShader->use();
	lightShader->setMatrix4("view", camera.getLookAtMatrix());
	lightShader->setMatrix4("projection", projection);

	for (Light* light : this->getLights()) {
		if (light->getActive())
			light->draw(*shader, *lightShader);
		else
			light->disableLight(*shader);
	};
}

void Scene::setupScene() {
	
	this->addLight(new PointLight(glm::vec3(0.0f, 0.2f, 10.0f)));
	this->addLight(new DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f), 0.5, 0.5));
	this->addModel(new Model("models/fortressScaled/noSky.obj", glm::vec3(0.0f, -2.0f, -15.0f), 1.0f, false));


	this->addShader("shaders/default.vs", "shaders/default.fs" );
	this->addShader("shaders/default.vs", "shaders/light.fs");
	this->addShader("shaders/outline.vs", "shaders/outline.fs");
	this->addShaderFolder("shaders/postProcessing");
	
	this->addCubemap("ocean", new CubeMap{ "models/skybox/ocean&mountains",std::vector<std::string>{"posx.jpg","negx.jpg","posy.jpg","negy.jpg","posz.jpg","negz.jpg"} }),
	
	this->addFramebuffer("framebuffer", new Framebuffer{ Scene::width, Scene::height, 2, false});
}



void Scene::addShader(std::string vertexPath, std::string fragmentPath) {
	int lastOf = fragmentPath.find_last_of('/');
	//skips '/'
	lastOf++;																	
	std::string name = fragmentPath.substr(lastOf, fragmentPath.find_last_of('.') - lastOf);
	this->m_shaders.insert({ name, new Shader{vertexPath, fragmentPath} });
}

void Scene::addShaderFolder(std::string folder) {
	
	std::vector<std::string> paths;
	std::string currentFile;
	std::string vertexPath;

	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		currentFile = entry.path().generic_string();
		if (entry.path().generic_string().find(".vertex") != std::string::npos)
			vertexPath = currentFile;
		else
			paths.push_back(currentFile);
	}
	
	for (const std::string& fragmentPath : paths)
		addShader(vertexPath, fragmentPath);
}

void Scene::addCubemap(std::string name, CubeMap* cubemap) {
	this->m_cubemaps.insert({ name, cubemap });
}
void Scene::addFramebuffer(std::string name, Framebuffer* framebuffer) {
	this->m_framebuffers.insert({ name, framebuffer });
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (!camera_control) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		camera.mouseUpdate(xoffset, yoffset);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		camera_control = !camera_control;
		glfwSetInputMode(window, GLFW_CURSOR, camera_control ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}

bool& Scene::getBool(std::string name) {
	return this->pProcessing.getBool(name);
}

PostProcessing& Scene::getProcessing() {
	return this->pProcessing;
}

Shader* Scene::findShader(std::string name) {
	return this->m_shaders.find(name)->second;
}

void Scene::addModel(Model* model) {
	this->m_models.push_back(model);
}

void Scene::addLight(Light* light) {
	this->m_lights.push_back(light);
}

std::vector<Light*> Scene::getLights() {
	return this->m_lights;
}

std::vector<Model*> Scene::getModels() {
	return this->m_models;
}