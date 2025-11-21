#include "chicken_coop_system.h"
#include "resource_loader.h"
#include "game_attributes.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdio>

// ============================================================================
// DECLARAÇÕES EXTERNAS (definidas em main.cpp ou outros módulos)
// ============================================================================

extern glm::vec3 GridToWorld(int gridX, int gridZ);
extern float GetGroundHeight(int gridX, int gridZ);
extern void DrawVirtualObject(const char* object_name);

extern glm::mat4 Matrix_Translate(float tx, float ty, float tz);
extern glm::mat4 Matrix_Scale(float sx, float sy, float sz);
extern glm::mat4 Matrix_Rotate_Y(float angle);

extern GLint g_model_uniform;
extern GLint g_object_id_uniform;

// ============================================================================
// CONSTANTES
// ============================================================================

static constexpr float kChickenCoopScale = 0.35f;
static constexpr float kChickenCoopYOffset = 0.6f;
static const glm::vec3 kChickenCoopPivot = glm::vec3(0.0f, 0.68f, 5.90f);

// ============================================================================
// ARMAZENAMENTO LOCAL
// ============================================================================

static std::vector<ChickenCoop> g_ChickenCoops;

// ============================================================================
// IMPLEMENTAÇÃO
// ============================================================================

void InitializeChickenCoops() {
    g_ChickenCoops.clear();
}

void AddChickenCoop(int gridX, int gridZ) {
    ChickenCoop coop;
    coop.gridX = gridX;
    coop.gridZ = gridZ;
    coop.rotation = 0.0f;
    coop.active = true;

    glm::vec3 basePos = GridToWorld(gridX, gridZ);
    float groundY = GetGroundHeight(gridX, gridZ);
    coop.worldPos = glm::vec3(basePos.x, groundY + kChickenCoopYOffset, basePos.z);

    g_ChickenCoops.push_back(coop);
}

void DrawChickenCoops() {
    if (g_ChickenCoops.empty()) {
        return;
    }

    for (const ChickenCoop& coop : g_ChickenCoops) {
        if (!coop.active) {
            continue;
        }

    glm::mat4 model = Matrix_Translate(coop.worldPos.x, coop.worldPos.y, coop.worldPos.z)
            * Matrix_Rotate_Y(coop.rotation)
            * Matrix_Scale(kChickenCoopScale, kChickenCoopScale, kChickenCoopScale)
            * Matrix_Translate(-kChickenCoopPivot.x, -kChickenCoopPivot.y, -kChickenCoopPivot.z);

        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, MODEL_CHICKEN_COOP);

        DrawVirtualObject("Cube.011_Cube.012");
    }
}
