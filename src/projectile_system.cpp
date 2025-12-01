#include "projectile_system.h"
#include "matrices.h"
#include "game_attributes.h"
#include "hud.h"
#include "collisions.h"
#include "enemy_system.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Globais externas
extern void DrawVirtualObject(const char* object_name);
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;
extern std::vector<Enemy> g_Enemies;

// Lista de projéteis
std::vector<Projectile> g_Projectiles;

void InitializeProjectiles() {
    g_Projectiles.clear();
}

void SpawnProjectile(glm::vec3 startPos, glm::vec3 direction, float damage) {
    Projectile p;
    p.position = startPos;
    p.direction = glm::normalize(direction);
    p.damage = damage;
    p.speed = 10.0f;
    p.active = true;
    
    g_Projectiles.push_back(p);
}

void UpdateProjectiles(float deltaTime) {
    for (auto& p : g_Projectiles) {
        if (!p.active) 
            continue;

        p.position += p.direction * p.speed * deltaTime;

        // Desativa projetil depois de alguma distância TODO: limitar isso melhor, por exemplo, pelo range ou tempo
        if (p.position.x < -50 || p.position.x > 50 || p.position.z < -50 || p.position.z > 50) {
            p.active = false;
        }
    }
    CheckProjectileCollisions();
}

void DrawAllProjectils() {
    for (Projectile& p : g_Projectiles) {
        if (!p.active)
            continue;

        glm::mat4 model = Matrix_Translate(p.position.x, p.position.y, p.position.z)
                        * Matrix_Scale(0.0008f, 0.0008f, 0.0008f);

        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, MODEL_EGG);
        DrawVirtualObject("Uncracked_Egg");
    }
}

void CheckProjectileCollisions() {
    const float PROJECTILE_RADIUS = 0.2f; // Raio do ovo
    
    for (auto& projectile : g_Projectiles) {
        if (!projectile.active) continue;
        
        // Cria esfera de colisão para o projétil
        Sphere projectileSphere = CreateSphere(projectile.position, PROJECTILE_RADIUS);
        
        for (auto& enemy : g_Enemies) {
            if (!enemy.active || enemy.health <= 0.0f) continue;
            
            // Calcula posição de renderização do inimigo (com yOffset)
            const EnemyRenderInfo& renderInfo = GetEnemyRenderInfo(enemy.type);
            glm::vec3 enemyRenderPos = glm::vec3(
                enemy.position.x,
                enemy.position.y + renderInfo.yOffset,
                enemy.position.z
            );
            
            // Cria esfera de colisão para o inimigo
            Sphere enemySphere = CreateSphere(enemyRenderPos, renderInfo.collisionRadius);
            
            // TESTE DE COLISÃO ESFERA-ESFERA
            if (TestSphereSphere(projectileSphere, enemySphere)) {
                // Aplica dano
                enemy.health -= projectile.damage;
                
               
                // Desativa projétil
                projectile.active = false;
                
                // Se o inimigo morreu
                if (enemy.health <= 0.0f) {
                    const EnemyAttributes& attrs = GetEnemyAttributes(enemy.type);
                    AddMoney(attrs.goldReward);
                         
                    enemy.active = false;
                }
                
                break; 
            }
        }
    }
}
