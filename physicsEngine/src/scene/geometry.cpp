#include "scene/geometry.hpp"

namespace _462 {

Geometry::Geometry():
    position( Vector3::Zero ),
    orientation( Quaternion::Identity ),
    scale( Vector3::Ones )
{

}

Geometry::~Geometry() { } 

}
