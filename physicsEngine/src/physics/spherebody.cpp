#include "physics/spherebody.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "scene/sphere.hpp"
#include <iostream>
#include <exception>
#include <algorithm>

#define EPSILON_POSITION 0.01
#define EPSILON_ANGLE 0.005

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

Vector3 SphereBody::step_velocity(double dt, double motion_damping) {
    Vector3 acceleration = force / mass;
    return velocity + acceleration * dt;
}

Vector3 SphereBody::step_angular_velocity(double dt, double motion_damping) {
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
    Vector3 deltaV = dt * force / mass;
    velocity += deltaV;

    if (length(velocity) < EPSILON_POSITION) {
        velocity = Vector3::Zero;
        return Vector3::Zero;
    }

    Vector3 k1, k2, k3, k4;
    k1 = dt * velocity;
    k2 = dt * step_velocity(dt/2.0, 0);
    k3 = dt * step_velocity(dt/2.0, 0);
    k4 = dt * step_velocity(dt, 0);
    Vector3 deltaX = 1/6.0 * k1 + 1/3.0 * k2 + 1/3.0 * k3 + 1/6.0 * k4;
    position += deltaX;
    // also change the position of the graphical object!
    sphere->position = position;

    return deltaX;
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
    Vector3 angular_acceleration = torque / (2.0/5.0 * mass * radius * radius);
    angular_velocity += angular_acceleration * dt;

    if (length(angular_velocity) < EPSILON_ANGLE) {
        angular_velocity = Vector3::Zero;
        return Vector3::Zero;
    }

    Vector3 k1, k2, k3, k4;
    k1 = dt * angular_velocity;
    k2 = dt * step_angular_velocity(dt/2.0, 0);
    k3 = dt * step_angular_velocity(dt/2.0, 0);
    k4 = dt * step_angular_velocity(dt, 0);
    Vector3 deltaTheta = 1/6.0 * k1 + 1/3.0 * k2 + 1/3.0 * k3 + 1/6.0 * k4;
    Quaternion r = Quaternion(deltaTheta, length(deltaTheta));
    orientation = r * orientation;
    sphere->orientation = orientation;

    //std::cout << angular_velocity << " " << deltaTheta << "quaternion "<< r << std::endl;

    return deltaTheta;
}

void SphereBody::apply_force( const Vector3& f, const Vector3& offset )
{
    // TODO apply force/torque to sphere
    force += f;
    torque += cross(offset, f);
}

}
