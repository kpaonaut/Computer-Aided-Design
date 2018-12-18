#ifndef _462_SCENE_GEOMETRY_HPP_
#define _462_SCENE_GEOMETRY_HPP_

#include "math/vector.hpp"
#include "math/color.hpp"
#include "math/quaternion.hpp"

namespace _462 {

class Geometry
{
public:
    Geometry();
    virtual ~Geometry();

    /*  
       World transformation are applied in the following order:
       1. Scale
       2. Orientation
       3. Position
    */

    // The world position of the object.
    Vector3 position;

    // The world orientation of the object.
    Quaternion orientation;

    // The world scale of the object.
    Vector3 scale;

    /** 
     * Renders this geometry using OpenGL in the local coordinate space.
     */
    virtual void render() const = 0;
};

}

#endif
