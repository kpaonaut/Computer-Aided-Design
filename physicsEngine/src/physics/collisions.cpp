#include "physics/collisions.hpp"
#include <cstdio>

namespace _462 {


bool collides( SphereBody& body1, SphereBody& body2, real_t collision_damping )
{
    // TODO detect collision. If there is one, update velocity
    Vector3 deltaP = body1.position - body2.position;
    Vector3 deltaV = body1.velocity - body2.velocity;
    if ((dot(deltaP, deltaV) < 0) && (length(deltaP) < body1.radius + body2.radius)) {
      Vector3 d = deltaP / length(deltaP);
      Vector3 v1p = body1.velocity - body2.velocity;
      Vector3 u2 = body2.velocity + 2 * d * body1.mass / (body1.mass + body2.mass) * dot(v1p, d);
      body1.velocity = body1.mass * body1.velocity + body2.mass * body2.velocity - body2.mass * u2;
      body2.velocity = u2;
      return true;
    }
    else
      return false;
}

bool collides( SphereBody& body1, TriangleBody& body2, real_t collision_damping )
{
  // TODO detect collision. If there is one, update velocity
  // first check if the sphere interferes with the three edges
  Vector3 v1 = body2.vertices[0];
  Vector3 v2 = body2.vertices[1];
  Vector3 v3 = body2.vertices[2];
  Vector3 normal = normalize(cross(v2 - v1, v3 - v1));

  real_t d = std::abs(dot(body1.position - v1, normal));
  // deltaP: the normal, pointing towards the sphere
  Vector3 deltaP = normal;
  if (dot(body1.position - v1, normal) < 0)
    deltaP = -normal;

  Vector3 deltaV = body1.velocity - body2.velocity;
  if ((d < body1.radius) && (dot(deltaP, deltaV) < 0)) {
    body1.velocity = body1.velocity - 2 * (dot(body1.velocity, normal)) * normal;
    return true;
  }
  else
    return false;
}

bool collides( SphereBody& body1, PlaneBody& body2, real_t collision_damping )
{
    // TODO detect collision. If there is one, update velocity
  real_t d = std::abs(dot(body1.position - body2.position, body2.normal));
  // deltaP: the normal, pointing towards the sphere
  Vector3 deltaP = body2.normal;
  if (dot(body1.position - body2.position, body2.normal) < 0)
    deltaP = -body2.normal;

  Vector3 deltaV = body1.velocity - body2.velocity;
  if ((d < body1.radius) && (dot(deltaP, deltaV) < 0)) {
    body1.velocity = body1.velocity - 2 * (dot(body1.velocity, body2.normal)) * body2.normal;
    return true;
  }
  else
    return false;
}


}