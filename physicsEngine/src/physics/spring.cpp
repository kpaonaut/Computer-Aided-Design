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
}

void Spring::step( real_t dt )
{
    // TODO apply forces to attached bodies
  // Vector3 f = -constant * (body1.position - eq);
  // body1.apply_force(f, body1_offset);
  // body2.apply_force(f, body2_offset);
}

}


