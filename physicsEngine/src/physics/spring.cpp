#include "math/math.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "physics/spring.hpp"
#include "physics/body.hpp"
#include "physics/spherebody.hpp"
#include <iostream>

namespace _462 {

Spring::Spring()
{
    body1_offset = Vector3::Zero;
    body2_offset = Vector3::Zero;
    damping = 0.0;
    prev_force = Vector3::Zero;
}

void Spring::step( real_t dt )
{
    // TODO apply forces to attached bodies
  Vector3 f = -constant / 100.0 * (length(body1->position - body2->position) - equilibrium) * normalize(body1->position - body2->position);
              - damping * (dot(body1->velocity - body2->velocity, normalize(body1->position - body2->position))) * normalize(body1->position - body2->position);
  
  //std::cout << constant << " " << (length(body1->position - body2->position) - equilibrium) * normalize(body1->position - body2->position)<<" " << f << std::endl;
  //std::cout << damping << std::endl;
  //std::cout << f << std::endl;
  //std::cout << length(body1->position - body2->position) << std::endl;
  //std::cout << body2->position << std::endl;

  Vector3 delta_force = f - prev_force;
  prev_force = f;

  body1->apply_force(delta_force, body1_offset);
  body2->apply_force(-delta_force, body2_offset);
}

}

