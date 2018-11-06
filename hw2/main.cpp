// Std. Includes
#include <string>

// GLAD
#include "../glad/glad.h"

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"

// GLM Mathemtics
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// Other Libs
#include "../SOIL.h"

#include "stars.h"

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint loadTexture(GLchar* path, GLboolean alpha = true);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];

// The MAIN function, from here we start our application and run our Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Blending1", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Options
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD to setup the OpenGL Function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup some OpenGL options
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
    
    // Setup and compile our shaders
    Shader shader("main.vert.glsl", "main.frag.glsl");
    
    // Set the object data (buffers, vertex attributes)
    GLfloat edgeLength = 0.07f;
    GLfloat transparentVertices[] = {
        // Positions                          // Texture Coords
        0.0f,  edgeLength,  0.0f,             0.0f,  1.0f,
        0.0f, -edgeLength,  0.0f,             0.0f,  0.0f,
        edgeLength * 2, -edgeLength,  0.0f,   1.0f,  0.0f,
        
        0.0f,  edgeLength,  0.0f,             0.0f,  1.0f,
        edgeLength * 2, -edgeLength,  0.0f,   1.0f,  0.0f,
        edgeLength * 2,  edgeLength,  0.0f,   1.0f,  1.0f
    };
    // Setup transparent plane VAO
    GLuint transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    
    // Load textures
    GLuint transparentTexture = loadTexture("Star.bmp", true);

    Stars elements = Stars(); // instantiate stars
    
    // color init
    glm::vec3 color[16];
    for (GLint i = 0; i < 5; i++) {
        color[i].r = 1.0f;
        color[i].g = i * 0.2f;
        color[i].b = 0.0f;
    }
    for (GLint i = 5; i < 10; i++) {
        color[i].r = 1.0f - (i - 4)*0.2f;
        color[i].g = 1.0f;
        color[i].b = 0.0f;
    }
    for (GLint i = 10; i < 16; i++) {
        color[i].r = (i - 10) * 0.2f;
        color[i].g = 1.0f - (i - 10)*0.2f;
        color[i].b = 1.0f;
    }

    // Game loop
    GLfloat deltaSpawnTime, lastFrame, deltaTime;
    GLfloat currentFrame = glfwGetTime();
    GLfloat lastSpawnTime = currentFrame;
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        elements.update(deltaTime);
        deltaSpawnTime = currentFrame - lastSpawnTime;
        if (deltaSpawnTime > 0.1) {
            elements.spawn();
            lastSpawnTime = currentFrame;
        }

        glfwPollEvents();
        
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Draw objects
        shader.Use();
        glm::mat4 model(1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // draw all elements
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (GLuint i = 0; i < elements.starCoordinates.size(); i++)
        {
            GLint varyingColorLoc = glGetUniformLocation(shader.Program, "varyingColor");
            GLint index = elements.colorIndex[i];
            //std::cout << color[index].r <<" "<< color[index].g <<" "<< color[index].b << std::endl;
            glUniform4f(varyingColorLoc, color[index].r, color[index].g, color[index].b, 1.0f); // pass color into fragment shader
            model = glm::mat4(1);
            model = glm::translate(model, elements.starCoordinates[i]);
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}

// This function loads a texture from file. Note: texture loading functions like these are usually
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio).
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path, GLboolean alpha)
{
    //Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, alpha ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
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
