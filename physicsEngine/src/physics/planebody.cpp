#include "physics/planebody.hpp"
#include <iostream>

namespace _462 {

PlaneBody::PlaneBody() 
{
    orientation = Quaternion::Identity;
}

Vector3 PlaneBody::step_position( real_t dt, real_t motion_damping )
{
    return Vector3::Zero;
}

Vector3 PlaneBody::step_orientation( real_t dt, real_t motion_damping )
{
    return Vector3::Zero;
}

void PlaneBody::apply_force( const Vector3& f, const Vector3& offset )
{
    return;
}

}
