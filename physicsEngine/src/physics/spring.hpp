#ifndef _462_SPRING_HPP_
#define _462_SPRING_HPP_

#include "physics/body.hpp"

namespace _462 {

class Spring
{
public:
    Spring();
    virtual ~Spring() {};
    void step( real_t dt );

    real_t constant;
    real_t equilibrium;
    real_t damping;
    Body* body1;
    Body* body2;
    Vector3 body1_offset;
    Vector3 body2_offset;

    Vector3 prev_force, prev_body1_offset, prev_body2_offset;
};

}

#endif
