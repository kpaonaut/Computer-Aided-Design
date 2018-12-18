#include "physics/spherebody.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "scene/sphere.hpp"
#include <iostream>
#include <exception>
#include <algorithm>

namespace _462 {

SphereBody::SphereBody( Sphere* geom )
{
    sphere = geom;
    position = sphere->position;
    radius = sphere->radius;
    orientation = sphere->orientation;
    mass = 0.0;
    velocity = Vector3::Zero;
    angular_velocity = Vector3::Zero;
    force = Vector3::Zero;
    torque = Vector3::Zero;
}

Vector3 SphereBody::step_velocity(Vector3 dx, double dt) {
    Vector3 acceleration = force / mass;
    return velocity + acceleration * dt;
}

Vector3 SphereBody::step_angular_velocity(Vector3 dx, double dt) {
    Vector3 angular_acceleration = torque / (2.0/5.0 * mass * radius * radius);
    return angular_velocity + angular_acceleration * dt;
}

Vector3 SphereBody::step_position( real_t dt, real_t motion_damping )
{
    // Note: This function is here as a hint for an approach to take towards
    // programming RK4, you should add more functions to help you or change the
    // scheme
    // TODO return the delta in position dt in the future
    // k1 = dt * velocity;
    // k2 = dt * velocity + dt / 2.0 * spheres[i].force / spheres[i].mass;
    // k3 = dt * 
    return velocity * dt;
}

Vector3 SphereBody::step_orientation( real_t dt, real_t motion_damping )
{
    // Note: This function is here as a hint for an approach to take towards
    // programming RK4, you should add more functions to help you or change the
    // scheme
    // TODO return the delta in orientation dt in the future
    // vec.x = rotation along x axis
    // vec.y = rotation along y axis
    // vec.z = rotation along z axis

    return Vector3::Zero;
}

void SphereBody::apply_force( const Vector3& f, const Vector3& offset )
{
    // TODO apply force/torque to sphere
    force += f;
    torque += cross(offset, f);
}

}
