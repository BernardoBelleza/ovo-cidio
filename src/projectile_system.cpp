#include "projectile_system.h"
#include "matrices.h"
#include "game_attributes.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Globais externas
extern void DrawVirtualObject(const char* object_name);
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;

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

