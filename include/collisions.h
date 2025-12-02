#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Sphere {
    glm::vec3 center;
    float radius;
};

struct Plane {
    glm::vec3 normal;
    float distance;
};


//  Teste Esfera-Esfera para colisões entre inimigos e projéteis
bool TestSphereSphere(const Sphere& s1, const Sphere& s2);

// Teste Ponto-Esfera para verificar se um inimigo está no range
bool TestPointSphere(const glm::vec3& point, const Sphere& sphere);

// Teste Esfera-Plano para limitar projeteis
bool TestSpherePlane(const Sphere& sphere, const Plane& plane);

Sphere CreateSphere(const glm::vec3& center, float radius);
Plane CreatePlane(const glm::vec3& normal, const glm::vec3& pointOnPlane);

#endif // COLLISIONS_H