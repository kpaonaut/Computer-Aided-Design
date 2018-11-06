#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <cstdlib>
#define PI 3.1415926f

class Stars {
private:
  //std::vector<glm::vec3> starCoordinates;
  int num, directionNum;
  GLfloat v, radiusLimit; // velocity
public:
    Stars();
    void spawn();
    void update(GLfloat dt);
    std::vector<glm::vec3> starCoordinates;
    std::vector<glm::vec3> directions;
    std::vector<GLint> colorIndex;
};

Stars::Stars() {
  num = 0;
  directionNum = 16; // divide the space into 10 pieces
  v = 0.2f;
  radiusLimit = 1.0f;
}

void Stars::spawn() {
  num = (num + 1) % directionNum; // 1~10
  starCoordinates.push_back(glm::vec3(0.0f,  0.0f,  0.0f));
  directions.push_back(glm::vec3(glm::cos(2 * PI / directionNum * num), glm::sin(2 * PI / directionNum * num), 0.0f));
  colorIndex.push_back(num);
  // std::cout<<glm::cos(2 * PI / directionNum * num)<<std::endl;
}

void Stars::update(GLfloat dt) {
  for (GLint i = 0; i < starCoordinates.size(); i++) {
    starCoordinates[i] += v * dt * directions[i];
  }
  if (starCoordinates.size() > 0) {
    while ((starCoordinates[0].x * starCoordinates[0].x) + (starCoordinates[0].y * starCoordinates[0].y) > radiusLimit * radiusLimit) {
        starCoordinates.erase(starCoordinates.begin()); // delete this coordinate: out of range!
        directions.erase(directions.begin());
        colorIndex.erase(colorIndex.begin());
      }
  }
}