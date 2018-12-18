#ifndef _462_PHYSICS_PLANE_HPP_
#define _462_PHYSICS_PLANE_HPP_

#include "physics/body.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"

namespace _462 {

class PlaneBody : public Body
{
public:
    PlaneBody();
    virtual ~PlaneBody() { }
    virtual Vector3 step_position( real_t dt, real_t motion_damping );
    virtual Vector3 step_orientation( real_t dt, real_t motion_damping );
    virtual void apply_force( const Vector3& f, const Vector3& offset );

    Vector3 normal;
};

}

#endif


