#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// Estruturas para representar formas geométricas
struct Sphere {
    glm::vec3 center;
    float radius;
};

//  Teste Esfera-Esfera
bool TestSphereSphere(const Sphere& s1, const Sphere& s2);


Sphere CreateSphere(const glm::vec3& center, float radius);

#endif // COLLISIONS_H