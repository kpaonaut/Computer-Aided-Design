#include "physics/physics.hpp"
#include <cstdio>
#include <iostream>

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
    //std::cout << "plane normal" << planes[0]->normal << "plane orientation" << planes[0]->orientation << std::endl;
    //std::cout << gravity << std::endl;
    //std::cout << num_triangles() << std::endl;
    for (int i = 0; i < spheres.size(); i++) {
        spheres[i]->step_position(dt, 0);
        spheres[i]->step_orientation(dt, 0);
    }

    for (int i = 0; i < num_spheres(); i++) {
        for (int j = i + 1; j < num_spheres(); j++) {
            collides(*(spheres[i]), *(spheres[j]), collision_damping);
        }
        //detect collision with plane && triangle
        for (int j = 0; j < num_planes(); j++) {
            collides(*(spheres[i]), *(planes[j]), collision_damping);
        }
        for (int j = 0; j < num_triangles(); j++) {
            collides(*spheres[i], *triangles[j], collision_damping);
        }
    }

    for (int i = 0; i < num_springs(); i++) {
        springs[i]->step(dt);

        springs[i]->body1->step_position(dt, 0);
        springs[i]->body1->step_orientation(dt, 0);

        springs[i]->body2->step_position(dt, 0);
        springs[i]->body2->step_orientation(dt, 0);

        //std::cout<< "1: velocity "<< springs[i]->body1->velocity << " position " << springs[i]->body1->position << std::endl;
        //std::cout<< "2: velocity "<< springs[i]->body2->velocity << " position " << springs[i]->body2->position << std::endl;
    }
}

void Physics::add_sphere( SphereBody* b )
{
    spheres.push_back( b );
    b->apply_force(gravity, Vector3::Zero);
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
