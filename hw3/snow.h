#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <cstdlib>
#include "camera.h"
#define PI 3.1415926f

class Particle {
private:
  GLfloat mass, dragCoefficient;
  glm::vec3 velocity, position, force;
  Camera* camera;
  glm::mat4 view;
public:
  void update(GLfloat deltaTime);
  glm::vec3 getPosition();
  Particle();
};

Particle::Particle(GLfloat m, glm::vec3 v, glm::vec3 p) {
  mass = m;
  velocity = v;
  position = p;
  force = glm::vec3(0.0f, -9.8f, 0.0) * mass; // initilize with gravity
  dragCoefficient = 10.0; // change this number!
}

void Particle::update(GLfloat deltaTime) {
  position = position + velocity * deltaTime;
  velocity = velocity + force / mass * deltaTime;
  force = glm::vec3(0.0f, -9.8f, 0.0) * mass - k * velocity * velocity;
}

glm::vec3 Particle::getPosition() {
  return position;
}