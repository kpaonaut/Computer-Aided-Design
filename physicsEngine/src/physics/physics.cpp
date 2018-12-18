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
    Vector3 k1, k2, k3, k4;
    //printf("position: %f %f %f\n", spheres[0]->position.x, spheres[0]->position.y, spheres[0]->position.z);
    //printf(velocity: "%f %f %f\n", spheres[0]->velocity.x, spheres[0]->velocity.y, spheres[0]->velocity.z);
    //printf("force: %f %f %f\n", spheres[0]->force.x, spheres[0]->force.y, spheres[0]->force.z);
    for (int i = 0; i < spheres.size(); i++) {
        Vector3 deltaV = dt * spheres[i]->force / spheres[i]->mass;
        spheres[i]->velocity += deltaV;

        k1 = dt * spheres[i]->velocity;
        k2 = dt * spheres[i]->step_velocity(k1/2.0, dt/2.0);
        k3 = dt * spheres[i]->step_velocity(k2/2.0, dt/2.0);
        k4 = dt * spheres[i]->step_velocity(k3, dt);
        Vector3 deltaX = 1/6.0 * k1 + 1/3.0 * k2 + 1/3.0 * k3 + 1/6.0 * k4;
        spheres[i]->position += deltaX;
        // also change the position of the graphical object!
        spheres[i]->sphere->position = spheres[i]->position;
        k1 = dt * spheres[i]->angular_velocity;
        k2 = dt * spheres[i]->step_angular_velocity(k1/2.0, dt/2.0);
        k3 = dt * spheres[i]->step_angular_velocity(k2/2.0, dt/2.0);
        k4 = dt * spheres[i]->step_angular_velocity(k3, dt);
        Vector3 deltaTheta = 1/6.0 * k1 + 1/3.0 * k2 + 1/3.0 * k3 + 1/6.0 * k4;
        Quaternion r = Quaternion(deltaTheta, length(deltaTheta));
        spheres[i]->orientation = r * spheres[i]->orientation;
        spheres[i]->sphere->orientation = spheres[i]->orientation;

        //detect collision with plane && triangle
        for (int j = 0; j < num_planes(); j++) {
            
        }
        for (int j = 0; j < num_triangles(); j++) {

        }
    }
}

void Physics::add_sphere( SphereBody* b )
{
    spheres.push_back( b );
    b->apply_force(gravity, Vector3(0.0, 0.0, 0.0));
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
