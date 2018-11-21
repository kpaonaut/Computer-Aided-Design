#include <iostream>
#include <cmath>
#include <random>
#include <ctime> 

// GLAD
#include "../glad/glad.h"

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// Other includes
#include "shader.h"
#include "camera.h"
#include "text.h"
// text.h should be included before parameters.h
#include "parameters.h"

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(-200.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Camera3", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // enable depth check
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");
    Shader textShader("text.vert.glsl", "text.frag.glsl");
    initFreetype();

    /***************************************************/
    /*
                              MERCURY   VENUS   EARTH   MOON    MARS    JUPITER   SATURN    URANUS    NEPTUNE 
    Diameter (km)             4879      12104   12756   3475    6792    142984    120536    51118     49528
    Distance from Sun (e5km)  579       1082    1496    3.84    2279    7786      14335     28725     44951  
    Orbital Period (days)     88.0      224.7   365.2   27.3    687.0   4331      10747     30589     59800

    SUN: 1391016 km

    MOON: 3474 km
    */
    Body sun = Body(13.91016, 1000, 0, "sun");
    Body mercury = Body(1.0, 8.8, 10, "mercury");
    Body venus = Body(0.5, 22.47, 20, "venus");
    Body earth = Body(1.0, 36.52, 30, "earth");
    Body moon = Body(0.5, 2.73, 1.0, "moon");
    Body mars = Body(0.8, 68.7, 40, "mars");
    Body jupiter = Body(3, 43, 60, "jupiter");
    Body saturn = Body(2, 107, 100, "saturn");
    Body uranus = Body(1, 305, 200, "uranus");
    Body neptune = Body(1, 598, 400, "neptune");

    /****************************************************/
    // Game loop

  GLuint textVAO, textVBO;
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glBindVertexArray(textVAO);
  glBindBuffer(GL_ARRAY_BUFFER, textVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        // sun.update(deltaTime);
        mercury.update(deltaTime);
        venus.update(deltaTime);
        earth.update(deltaTime);
        moon.update(deltaTime);
        mars.update(deltaTime);
        jupiter.update(deltaTime);
        saturn.update(deltaTime);
        uranus.update(deltaTime);
        neptune.update(deltaTime);

        lastFrame = currentFrame;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Render
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        // Activate shader
        ourShader.Use();
        //textShader.Use();

        // Camera/View transformation
        // Projection
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);
        // Get the uniform locations
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // model is set in body.draw()

        sun.draw(ourShader);
        mercury.draw(ourShader);
        venus.draw(ourShader);
        earth.draw(ourShader);
        moon.draw(ourShader, &earth);
        mars.draw(ourShader);
        jupiter.draw(ourShader);
        saturn.draw(ourShader);
        uranus.draw(ourShader);
        neptune.draw(ourShader);

        textShader.Use();
        // Get the uniform locations
        viewLoc = glGetUniformLocation(textShader.Program, "view");
        projLoc = glGetUniformLocation(textShader.Program, "projection");
        // Pass the matrices to the text shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        sun.drawText(textShader);
        mercury.drawText(textShader);
        venus.drawText(textShader);
        earth.drawText(textShader);
        moon.drawText(textShader, &earth);
        mars.drawText(textShader);
        jupiter.drawText(textShader);
        saturn.drawText(textShader);
        uranus.drawText(textShader);
        neptune.drawText(textShader);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement() // absolute coordinates here, better change to rotation around local coordinates
{
    // camera rotation
    if (keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (keys[GLFW_KEY_LEFT])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_RIGHT])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_N])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_M])
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (keys[GLFW_KEY_A])
        camera.ProcessMouseMovement(100.0 * deltaTime, 0);
    if (keys[GLFW_KEY_D])
        camera.ProcessMouseMovement(-100.0 * deltaTime, 0);
    if (keys[GLFW_KEY_W])
        camera.ProcessMouseMovement(0, 100.0 * deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessMouseMovement(0, -100.0 * deltaTime);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
