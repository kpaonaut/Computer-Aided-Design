#ifndef _462_COLLISIONS_HPP_
#define _462_COLLISIONS_HPP_

#include "scene/sphere.hpp"
#include "physics/spherebody.hpp"
#include "physics/trianglebody.hpp"
#include "physics/planebody.hpp"

namespace _462 {

bool collides( SphereBody& body1, SphereBody& body2, real_t collision_damping );
bool collides( SphereBody& body1, TriangleBody& body2, real_t collision_damping );
bool collides( SphereBody& body1, PlaneBody& body2, real_t collision_damping );

}

#endif
