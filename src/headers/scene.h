#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "model.h"
#include "gui.h"
#include "light.h"
#include "framebuffer.h"
#include "cubemap.h"
#include "postProcessing.h"

class Scene {
    
public:

    Scene(uint16_t width, uint16_t height);

    /**
     * @brief Destructor cleans the libraries
    */
    ~Scene();
    
    /**
     * @brief Launch the render loop, doesnt initialize anything, just launching the loop
     * 
     */
    void renderLoop();

    /**
     * @brief Add a model to the scene
     * 
     * @param model
    */
    void addModel(Model*);

    /**
     * @brief Remove a light to the scene
     * 
     * @param light
    */
    void addLight(Light*);

    /**
     * @brief Adds a shader to the m_shaders map, taking as key the name of the fragment shader
     *
     * @param light
    */
    void addShader(std::string vertexPath, std::string fragmentPath);
    void addShaderFolder(std::string folder);
    void addCubemap(std::string name, CubeMap* cubemap);
    void addFramebuffer(std::string name, Framebuffer* framebuffer);
    void setupScene();

    std::vector<Light*> getLights();
    std::vector<Model*> getModels();
    PostProcessing& getProcessing();
    Shader* findShader(std::string name);
    bool& getBool(std::string name);

    static uint16_t width;
    static uint16_t height;

private:
    GLFWwindow* m_pWindow;
    DefaultGui m_gui;
    // bool renderOptions_draw_lights = true;
    std::vector<Light*> m_lights;
    std::vector<Model*> m_models;
    std::map<std::string, Shader*> m_shaders;
    std::map<std::string, CubeMap*> m_cubemaps;
    std::map<std::string, Framebuffer*> m_framebuffers;
    PostProcessing pProcessing{};

    /**
     * @brief Init all the libraries and generate a windows
     * 
     * @return GLFWwindow* 
     */
    GLFWwindow* initWindow();

    /**
     * @brief init the GLFW Library : 
     * GLFW provides a simple API for creating windows, contexts and surfaces, receiving input and events
     */
    void initGLFW();

    /**
     * @brief create a window from glfw
     * 
     * @return GLFWwindow* program can exit if window finish as null
     */
    GLFWwindow* createWindow();

    /**
     * @brief Init the GLAD Library : 
     * GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function.
     */
    void initGLAD();
    
    /**
     * @brief Setup a callback for when the user resize the window
     * 
     * @param window 
     * @param width 
     * @param height 
     */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Scene::width = width;
        Scene::height = height;
    }

    void drawScene(std::string shaderName);
    
    void setCallBacks();
    void setOpenGLOptions();
};

extern Camera camera;

#endif