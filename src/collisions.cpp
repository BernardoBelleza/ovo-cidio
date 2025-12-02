#include "collisions.h"
#include <cmath>
#include <glm/glm.hpp>

bool TestSphereSphere(const Sphere& s1, const Sphere& s2) {
    glm::vec3 diff = s2.center - s1.center;

    float distanceSquared = glm::dot(diff, diff);
    
    float radiusSum = s1.radius + s2.radius;
    
    // Se a distância for menor que a distancia dos raios, há colisão
    return distanceSquared <= (radiusSum * radiusSum);
}

bool TestPointSphere(const glm::vec3& point, const Sphere& sphere) {
    glm::vec3 diff = point - sphere.center;

    float distanceSquared = glm::dot(diff, diff);

    return distanceSquared <= (sphere.radius * sphere.radius);
}

Sphere CreateSphere(const glm::vec3& center, float radius) {
    Sphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    return sphere;
}

bool TestSpherePlane(const Sphere& sphere, const Plane& plane) {

    float distance = glm::dot(plane.normal, sphere.center) - plane.distance;
    
    return std::abs(distance) <= sphere.radius;
}

Plane CreatePlane(const glm::vec3& normal, const glm::vec3& pointOnPlane) {
    Plane plane;
    plane.normal = glm::normalize(normal);
    plane.distance = glm::dot(plane.normal, pointOnPlane);
    return plane;
}
