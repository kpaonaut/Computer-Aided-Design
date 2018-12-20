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
  //std::cout << body1_offset << std::endl;
  Matrix3 rotation_martix = Matrix3::Identity;
  body1->orientation.to_matrix(&rotation_martix);
  Vector3 body1_offset_rotated = rotation_martix * body1_offset;
  body2->orientation.to_matrix(&rotation_martix);
  Vector3 body2_offset_rotated = rotation_martix * body2_offset;

  Vector3 displacement = body1->position + body1_offset_rotated - body2->position - body2_offset_rotated;
  Vector3 f = - constant * (length(displacement) - equilibrium) * normalize(displacement);
              - damping * (dot(body1->velocity - body2->velocity, normalize(displacement))) * normalize(displacement);
  
  //std::cout << constant << " " << (length(body1->position - body2->position) - equilibrium) * normalize(body1->position - body2->position)<<" " << f << std::endl;
  //std::cout << damping << std::endl;
  //std::cout << f << std::endl;
  //std::cout << length(body1->position - body2->position) << std::endl;
  //std::cout << body2->position << std::endl;

  // Vector3 delta_force = f - prev_force;
  // Vector3 delta_torque = cross(body1_offset_rotated, f) - prev_torque;

  body1->apply_force(-prev_force, prev_body1_offset);
  body1->apply_force(f, body1_offset_rotated);

  body2->apply_force(prev_force, prev_body2_offset);
  body2->apply_force(-f, body2_offset_rotated);

  prev_body1_offset = body1_offset_rotated;
  prev_body2_offset = body2_offset_rotated;
  prev_force = f;
}

}

