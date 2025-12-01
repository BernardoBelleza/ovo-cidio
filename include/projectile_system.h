#ifndef PROJECTILE_SYSTEM_H
#define PROJECTILE_SYSTEM_H

#include <glm/vec3.hpp>
#include <vector>
#include "enemy_system.h" // Para saber quem é o alvo

// Definição do Projétil
struct Projectile {
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float damage;
    bool active;
};

// Funções do sistema
void InitializeProjectiles();
void SpawnProjectile(glm::vec3 startPos, glm::vec3 direction, float damage);
void UpdateProjectiles(float deltaTime);
void DrawAllProjectils();

void CheckProjectileCollisions();

#endif // PROJECTILE_SYSTEM_H