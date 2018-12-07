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
#include "parse_obj.h"

// define different display modes
#define MODE_FILL 0
#define MODE_LINE 1
#define MODE_POINT 2
#define MODE_FILL_AND_LINE 3

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement(glm::mat4 &model);

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.frag.glsl");

    /***************************************************/
    GLfloat *verticesPointer;
    int numFaces;
    char filename[] = "eight.uniform.obj";
    verticesPointer = readObjFile(filename, numFaces);

    glm::vec3 initialPosition = glm::vec3(0.0f,  0.0f,  0.0f);
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*verticesPointer)*numFaces*15, verticesPointer, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind VAO
    
    // initialize model attitude & position (transform)
    glm::mat4 model(1);
    model = glm::translate(model, initialPosition); // no need to translate here
    GLfloat angle = 0.0f;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));

    // light source position
    glm::vec3 lightPos(0.8f, 1.0f, -0.2f);

    // initilize color for fill mode
    glm::vec4 varyingColor = glm::vec4(0.0f, 0.5f, 0.2f, 1.0f);
    GLfloat *rValue = new GLfloat[numFaces];
    GLfloat *gValue = new GLfloat[numFaces];
    GLfloat *bValue = new GLfloat[numFaces];
    srand((unsigned)time(0));
    for (GLint i = 0; i < numFaces; i ++){
        rValue[i] = (std::rand() % 100 + 0.0) / 100.0;
        gValue[i] = (std::rand() % 100 + 0.0) / 100.0;
        bValue[i] = (std::rand() % 100 + 0.0) / 100.0;
    }

    /****************************************************/
    // Game loop  
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement(model);

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        
        // Activate shader
        ourShader.Use();
        // Camera/View transformation
        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        // Projection
        glm::mat4 projection(1);
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
        GLint varyingColorLoc = glGetUniformLocation(ourShader.Program, "varyingColor");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        //glPolygonMode(GL_FRONT, GL_FILL);

        // Calculate the model matrix for each object and pass it to shader before drawing
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // set model as transformation

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (int i = 0; i < numFaces; i ++){
            glUniform4f(varyingColorLoc, rValue[i], gValue[i], bValue[i], 1.0f); // pass color into fragment shader
            glDrawArrays(GL_TRIANGLES, i * 3, 3);
        }

        glBindVertexArray(0); // unbind
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

void do_movement(glm::mat4 &model) // absolute coordinates here, better change to rotation around local coordinates
{
    // object rotation
    GLfloat angle = 50.0f * deltaTime;
    GLfloat coord = 1.0f * deltaTime;
    if (keys[GLFW_KEY_UP])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    if (keys[GLFW_KEY_DOWN])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(-1.0f, 0.0f, 0.0f));
    if (keys[GLFW_KEY_LEFT])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    if (keys[GLFW_KEY_RIGHT])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, -1.0f));
    if (keys[GLFW_KEY_N])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 0.0f));
    if (keys[GLFW_KEY_M])
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, -1.0f, 0.0f));

    if (keys[GLFW_KEY_A])
        model = glm::translate(model, glm::vec3(coord,  0.0f,  0.0f));
    if (keys[GLFW_KEY_D])
        model = glm::translate(model, glm::vec3(-coord,  0.0f,  0.0f));
    if (keys[GLFW_KEY_W])
        model = glm::translate(model, glm::vec3(0.0f,  coord,  0.0f));
    if (keys[GLFW_KEY_S])
        model = glm::translate(model, glm::vec3(0.0f,  -coord,  0.0f));
    if (keys[GLFW_KEY_Z])
        model = glm::translate(model, glm::vec3(0.0f,  0.0f,  coord));
    if (keys[GLFW_KEY_X])
        model = glm::translate(model, glm::vec3(0.0f,  0.0f,  -coord));
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    return; // cancel mouse effects
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
