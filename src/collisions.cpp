#include "collisions.h"
#include <cmath>

bool TestSphereSphere(const Sphere& s1, const Sphere& s2) {
    glm::vec3 diff = s2.center - s1.center;

    float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    
    float radiusSum = s1.radius + s2.radius;
    
    // Se a distância for menor que a distancia dos raios, há colisão
    return distanceSquared <= (radiusSum * radiusSum);
}

Sphere CreateSphere(const glm::vec3& center, float radius) {
    Sphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    return sphere;
}