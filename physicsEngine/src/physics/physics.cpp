#include "physics/physics.hpp"

namespace _462 {

Physics::Physics()
{
    reset();
}

Physics::~Physics()
{
    reset();
}

void Physics::step( real_t dt )
{
    // TODO step the world forward by dt. Need to detect collisions, apply
    // forces, and integrate positions and orientations.
    //
    // Note: put RK4 here, not in any of the physics bodies
    //
    // Must use the functions that you implemented
    //
    // Note, when you change the position/orientation of a physics object,
    // change the position/orientation of the graphical object that represents
    // it
    // k1 = dt * spheres[i].velocity;
    // k2 = dt * spheres[i].velocity + dt / 2.0 * spheres[i].force / spheres[i].mass;
    // k3 = 
    // for (int i = 0; i < spheres.size(); i++) {
    //     spheres[i].step_position(dt, dt);
    //     spheres[i].step_orientation(dt, dt);
    // }
}

void Physics::add_sphere( SphereBody* b )
{
    spheres.push_back( b );
}

size_t Physics::num_spheres() const
{
    return spheres.size();
}

void Physics::add_plane( PlaneBody* p )
{
    planes.push_back( p );
}

size_t Physics::num_planes() const
{
    return planes.size();
}

void Physics::add_triangle( TriangleBody* t )
{
    triangles.push_back( t );
}

size_t Physics::num_triangles() const
{
    return triangles.size();
}

void Physics::add_spring( Spring* s )
{
    springs.push_back( s );
}

size_t Physics::num_springs() const
{
    return springs.size();
}

void Physics::reset()
{
    for ( SphereList::iterator i = spheres.begin(); i != spheres.end(); i++ ) {
        delete *i;
    }
    for ( PlaneList::iterator i = planes.begin(); i != planes.end(); i++ ) {
        delete *i;
    }
    for ( TriangleList::iterator i = triangles.begin(); i != triangles.end(); i++ ) {
        delete *i;
    }
    for ( SpringList::iterator i = springs.begin(); i != springs.end(); i++ ) {
        delete *i;
    }

    spheres.clear();
    planes.clear();
    triangles.clear();
    springs.clear();
    
    gravity = Vector3::Zero;
	collision_damping = 0.0;
}

}
