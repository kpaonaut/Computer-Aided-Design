#include <iostream>

// GLAD
#include "../glad/glad.h"

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "../SOIL.h"

// Other includes
#include "shader.h"
#include "camera.h"

// GLM Mathemtics
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void change_scale();
void Do_Movement();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

bool keys[1024];
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float level = 5.0f;
int drawMode = 1;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bezier2", nullptr, nullptr);
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
    
    // Build and compile our shader program
    Shader ourShader("main.vert.glsl", "main.tcs.glsl", "main.tes.glsl", "main.frag.glsl");
    Shader ourShader2("main.vert.glsl", "main.frag2.glsl");
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    // 25 control points
    GLfloat vertices[] = {
        -1.5, 2., -3.,
        -0.5, 1., -3.,
        0.5, 1., -3.,
        1.5, 2., -3.,
        2.0, 2., -3.,
        
        -1.5, 1., -2.,
        -0.5, -2., -2.,
        0.5, 1., -2.,
        1.5, 0., -2.,
        2.5, 0., -2.,
        
        -1.5, 0., -1.,
        -0.5, 1., -1.,
        0.5, 0., -1.,
        1.5, -1., -1.,
        2.0, -1., -1.,
        
        -1.5, 0., 0.,
        -0.5, 1., 0.,
        0.5, -1., 0.,
        1.5, -1., 0.,
        2.0, -1., 0.,

        -1.5, 0., 1.,
        -0.5, 1., 1.,
        0.5, -1., 1.,
        1.5, -1., 1.,
        2.0, -1., 1.
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0); // Unbind VAO
    
    // Load and create a texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char* image = SOIL_load_image("texture.png", &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        change_scale();
        Do_Movement();
        
        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glm::mat4 model(1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
        // Activate shader
        ourShader.Use();
        glUniform1f(glGetUniformLocation(ourShader.Program, "uOuter02"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uOuter13"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uInner0"), level);
        glUniform1f(glGetUniformLocation(ourShader.Program, "uInner1"), level);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Draw bezier surface
        switch (drawMode) {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case 1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
        }
                
        glBindVertexArray(VAO);
        glPatchParameteri(GL_PATCH_VERTICES, 25);
        glDrawArrays(GL_PATCHES, 0, 25);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Draw control points
        ourShader2.Use();
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glPointSize(10.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 25);
        glBindVertexArray(0);
        
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

void change_scale()
{
    if (keys[GLFW_KEY_Z] && level > 1){
        if (level <= 20.0f)
            level -= deltaTime * 5.0f;
        else
            level -= deltaTime * 10.0f;
        level = level <= 1.0f ? 1.0f : level;
        std::cout << "\rLevel: " << level << "    ";
    }
    if (keys[GLFW_KEY_X] && level < 40){
        if (level < 20.0f)
            level += deltaTime * 5.0f;
        else
            level += deltaTime * 10.0f;
        level = level >= 40.0f ? 40.0f : level;
        std::cout << "\rLevel: " << level << "    ";
    }
    if (keys[GLFW_KEY_C]){
        drawMode = 1 - drawMode;
        std::cout << "\rDrawMode: " << drawMode << "    ";
        keys[GLFW_KEY_C] = false;
    }
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_R])
        camera.ProcessKeyboard(UP, deltaTime);
    if (keys[GLFW_KEY_F])
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    return;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

