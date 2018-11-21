/*
                          MERCURY   VENUS   EARTH   MOON    MARS    JUPITER   SATURN    URANUS    NEPTUNE 
Diameter (km)             4879      12104   12756   3475    6792    142984    120536    51118     49528
Distance from Sun (e6km)  57.9      108.2   149.6   0.384   227.9   778.6     1433.5    2872.5    4495.1  
Orbital Period (days)     88.0      224.7   365.2   27.3    687.0   4331      10747     30589     59800

SUN: 1391016 km

MOON: 3474 km    27.0 days

*/

#include <GLFW/glfw3.h>
// GLM Mathematics
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../SOIL.h"

#include "shader.h"
#include "text.h"

#include <string>
#include <cstdio>
#include <iostream>

//#include "../freetype/ft2build.h"
//#include FT_FREETYPE_H

#define PI 3.1416
#define SEGMENT_NUM 20 // how many segments are latitude and longitude divided into

class Body { // planets and the sun
private:
  GLfloat bodyRadius, omega, x, z, orbitRadius, theta;
  std::string name; // load texture easier: texture_lib/name.jpg
  GLuint* indices; // triangle indices for sphere
  GLfloat* vertices;
  GLuint numVertices, numIndices, numTriangles, texture;
  GLuint VBO, VAO, EBO;
  GLuint textVAO, textVBO;

public:
  Body(GLfloat diameter, GLfloat period, GLfloat orbitRadius, std::string nameValue);
  void update(GLfloat deltaTime);
  glm::vec3 getCoordinates(Body*);
  void allocateBuffer();
  void draw(Shader &ourShader, Body* base);
  void generateMesh();
  GLuint loadTexture(GLchar* path);
  void drawText(Shader &shader, Body* base);
  ~Body();

};

// for moon coord, just add offset from earth!

Body::Body(GLfloat diameter, GLfloat period, GLfloat orbitRadiusValue, std::string nameValue) {
  orbitRadius = orbitRadiusValue;
  name = nameValue;
  bodyRadius = diameter / 2;
  omega = 2.0 * PI / period;
  z = 0.0;
  x = 0.0 + orbitRadius;
  // load texture
  std::string textureImageName = "texture_lib/" + name + ".jpg";
  GLchar *imageName = new char[textureImageName.length() + 1];
  strcpy(imageName, textureImageName.c_str()); // textureImageName.c_str() is const
  texture = loadTexture(imageName);

  generateMesh();
  allocateBuffer();
}

Body::~Body() {
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &textVAO);
  glDeleteBuffers(1, &textVBO);
}

void Body::generateMesh() {
  GLuint numResoltion;
  if (name == "sun")
    numResoltion = SEGMENT_NUM * 10;
  else
    numResoltion = SEGMENT_NUM;
  // divide latitude and longitude into numResoltion segments
  numTriangles = numResoltion * numResoltion * 2;
  numIndices = 3 * numTriangles;
  numVertices = (numResoltion + 1) * (numResoltion + 1) * 5; // each vertex has three coords + two texture coords
  vertices = new GLfloat[numVertices];
  indices = new GLuint[numIndices];
  GLint tot = 0;

  for (GLint i = 0; i < numResoltion + 1; i ++) {
    for (GLint j = 0; j < numResoltion + 1; j ++) {
      GLfloat longitude = 1.0 / numResoltion * j; // 0.0 ~ 1.0
      GLfloat latitude = 1.0 / numResoltion * i;
      GLfloat localY = bodyRadius * glm::sin((latitude - 0.5) * PI);
      GLfloat smallRadius = bodyRadius * glm::cos((latitude - 0.5) * PI);
      GLfloat localX = smallRadius * glm::cos(longitude * 2 * PI);
      GLfloat localZ = smallRadius * glm::sin(longitude * 2 * PI);
      //std::cout << "longitude: " << longitude << " "<< longitude * 360 << "latitude: " <<latitude<<" "<< (latitude-0.5) * 180 << std::endl;
      vertices[tot * 5]     = localX;
      vertices[tot * 5 + 1] = localY;
      vertices[tot * 5 + 2] = localZ;
      vertices[tot * 5 + 3] = longitude;
      vertices[tot * 5 + 4] = latitude;
      tot ++;
    }
  }

  tot = 0;
  while (tot < numTriangles / 2) {
    for (GLuint j = 0; j < numResoltion; j ++) {
      indices[tot * 6] = (numResoltion + 1) * (tot / numResoltion) + j;
      indices[tot * 6 + 1] = indices[tot * 6] + numResoltion + 1;
      indices[tot * 6 + 2] = indices[tot * 6] + 1;
      indices[tot * 6 + 3] = indices[tot * 6 + 1] + 1;
      indices[tot * 6 + 4] = indices[tot * 6 + 1];
      indices[tot * 6 + 5] = indices[tot * 6 + 2];
      tot ++;
    }
  }
}

void Body::update(GLfloat deltaTime) {
  theta += deltaTime * omega;
  if (theta > 2 * PI) theta -= 2 * PI;
}

glm::vec3 Body::getCoordinates(Body* base = NULL) {
  x = orbitRadius * glm::cos(theta);
  z = orbitRadius * glm::sin(theta);
  if (name == "moon")
    return glm::vec3(x, 0, z) + base->getCoordinates(); // add earth coordinates
  else
    return glm::vec3(x, 0, z);
}

void Body::allocateBuffer() {
  // for body texture
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertices, vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numIndices, indices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glBindVertexArray(0);

  // for text texture
  glGenVertexArrays(1, &textVAO);
  glGenBuffers(1, &textVBO);
  glBindVertexArray(textVAO);
  glBindBuffer(GL_ARRAY_BUFFER, textVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Body::draw(Shader &ourShader, Body* base = NULL) {
  // should be different for moon!
  // draw background
  glBindTexture(GL_TEXTURE_2D, texture);
  glm::mat4 model(1);
  model = glm::translate(model, getCoordinates(base));
  glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0); // 40 triangles, 120 vertices in total
  glBindVertexArray(0);
}

void Body::drawText(Shader &shader, Body* base = NULL) {
  glm::mat4 model(1);
  model = glm::translate(model, getCoordinates(base));
  //std::cout<<getCoordinates().x<<" "<<getCoordinates().y<<" "<<getCoordinates().z<<std::endl;
  model = glm::translate(model, glm::vec3(0, bodyRadius * 1.1, 0)); // dispostition from body center
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  RenderText(shader, name, 0.0f, 0.0f, 0.01f, glm::vec3(0.7, 0.7f, 0.1f), textVAO, textVBO);
}

GLuint Body::loadTexture(GLchar* path) {
  //Generate texture ID and load texture data
  GLuint textureID;
  glGenTextures(1, &textureID);
  int width,height;
  unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
  // Assign texture to ID
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  // Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );    // Use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes value from next repeat
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  SOIL_free_image_data(image);
  return textureID;
}