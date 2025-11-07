#include "tower_system.h"
#include "model_loader.h"
#include <cstdio>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

// ============================================================================
// DECLARAÇÕES EXTERNAS (funções e variáveis definidas em main.cpp)
// ============================================================================

// Funções de grid (definidas em main.cpp)
extern glm::vec3 GridToWorld(int gridX, int gridZ);
extern glm::ivec2 WorldToGrid(glm::vec3 worldPos);
extern float GetGroundHeight(int gridX, int gridZ);

// Enumeração de tipos de célula do grid (definida em main.cpp)
// MAP_WIDTH e MAP_HEIGHT agora precisam ser variáveis exportadas
enum CellType {
    CELL_EMPTY = 0,
    CELL_PATH = 1,
    CELL_BLOCKED = 2,
    CELL_BASE = 3
};
extern int g_MapGrid[15][15];

// Funções de renderização (definidas em main.cpp)
extern void DrawVirtualObject(const char* object_name);

// Funções de transformação de matrizes (definidas em matrices.cpp)
extern glm::mat4 Matrix_Translate(float tx, float ty, float tz);
extern glm::mat4 Matrix_Scale(float sx, float sy, float sz);
extern glm::mat4 Matrix_Rotate_Y(float angle);
extern glm::mat4 Matrix_Rotate_X(float angle);
extern glm::mat4 Matrix_Rotate_Z(float angle);

// Variáveis globais de uniforms (definidas em main.cpp)
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;

// IDs dos modelos já estão definidos em model_loader.h como #defines

// Estrutura de arma anexada (definida em main.cpp)
struct AttachedWeapon {
    glm::vec3 offset;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool enabled;
};
extern AttachedWeapon g_ChickenWeapon;

// ============================================================================
// CONSTANTES FÍSICAS
// ============================================================================

const float GRAVITY = -9.8f;
const float DAMPING = 0.6f;
const float MIN_VELOCITY = 0.05f;
const float CHICKEN_Y_OFFSET = -0.43f;

// ============================================================================
// VARIÁVEIS GLOBAIS DO SISTEMA DE TORRES
// ============================================================================

Tower g_Towers[MAX_TOWERS];
int g_TowerCount = 0;

// ============================================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES
// ============================================================================

void InitializeTowers() {
    for (int i = 0; i < MAX_TOWERS; i++) {
        g_Towers[i].active = false;
        g_Towers[i].gridX = 0;
        g_Towers[i].gridZ = 0;
        g_Towers[i].physics.position = glm::vec3(0.0f, 0.0f, 0.0f);
        g_Towers[i].physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        g_Towers[i].physics.mass = 1.0f;
        g_Towers[i].physics.radius = 0.3f;
        g_Towers[i].physics.onGround = false;
    }
    g_TowerCount = 0;
    printf("[TORRE] Sistema de torres inicializado (max: %d)\n", MAX_TOWERS);
}

bool AddTower(int gridX, int gridZ) {
    if (g_TowerCount >= MAX_TOWERS) {
        printf("[TORRE] Limite maximo atingido!\n");
        return false;
    }
    
    // Verifica se ja existe torre nessa posicao
    for (int i = 0; i < g_TowerCount; i++) {
        if (g_Towers[i].active && g_Towers[i].gridX == gridX && g_Towers[i].gridZ == gridZ) {
            printf("[TORRE] Ja existe torre em (%d, %d)\n", gridX, gridZ);
            return false;
        }
    }
    
    // Converte posicao do grid para world
    glm::vec3 worldPos = GridToWorld(gridX, gridZ);
    
    // Adiciona nova torre
    g_Towers[g_TowerCount].gridX = gridX;
    g_Towers[g_TowerCount].gridZ = gridZ;
    g_Towers[g_TowerCount].active = true;
    
    // Inicializa fisica da torre (spawna 3 blocos acima do chao)
    g_Towers[g_TowerCount].physics.position = glm::vec3(worldPos.x, worldPos.y + 3.0f, worldPos.z);
    g_Towers[g_TowerCount].physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_Towers[g_TowerCount].physics.mass = 1.0f;
    g_Towers[g_TowerCount].physics.radius = 0.3f;
    g_Towers[g_TowerCount].physics.onGround = false;
    
    g_TowerCount++;
    
    printf("[TORRE] Torre adicionada em (%d, %d) - Total: %d/%d\n", 
           gridX, gridZ, g_TowerCount, MAX_TOWERS);
    return true;
}

void UpdatePhysics(PhysicsObject& obj, float deltaTime) {
    if (obj.onGround && glm::abs(obj.velocity.y) < MIN_VELOCITY) {
        obj.velocity.y = 0.0f;
        return; // Objeto parou no chão
    }
    
    // Aplica gravidade
    obj.velocity.y += GRAVITY * deltaTime;
    
    // Atualiza posição
    obj.position += obj.velocity * deltaTime;
    
    // Verifica colisão com o chão
    glm::ivec2 gridPos = WorldToGrid(obj.position);
    float groundHeight = GetGroundHeight(gridPos.x, gridPos.y);
    
    if (obj.position.y - obj.radius <= groundHeight) {
        // Colidiu com o chão
        obj.position.y = groundHeight + obj.radius;
        
        // Inverte velocidade com damping (quique)
        obj.velocity.y = -obj.velocity.y * DAMPING;
        
        // Se a velocidade for muito pequena, para o objeto
        if (glm::abs(obj.velocity.y) < MIN_VELOCITY) {
            obj.velocity.y = 0.0f;
            obj.onGround = true;
        }
    } else {
        obj.onGround = false;
    }
}

void UpdateAllTowersPhysics(float deltaTime) {
    for (int i = 0; i < g_TowerCount; i++) {
        if (g_Towers[i].active) {
            UpdatePhysics(g_Towers[i].physics, deltaTime);
        }
    }
}

void DrawChickenWithWeapon(glm::vec3 position, bool drawWeapon) {
    // Aplica offset Y para ajustar a base do modelo
    position.y += CHICKEN_Y_OFFSET;
    
    // Matriz de transformação da galinha
    glm::mat4 chickenModel = Matrix_Translate(position.x, position.y, position.z)
                           * Matrix_Scale(0.1f, 0.1f, 0.1f)
                           * Matrix_Rotate_Y(3.14159f / 2);
    
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(chickenModel));
    glUniform1i(g_object_id_uniform, MODEL_CHICKEN_TOWER);
    DrawVirtualObject("Low_Poly_Chicken_v1_001");
    
    // Desenha a arma se estiver habilitada
    if (drawWeapon && g_ChickenWeapon.enabled) {
        // Matriz de transformação da Thompson (aplicada à matriz da galinha)
        glm::mat4 weaponModel = chickenModel
                              * Matrix_Translate(g_ChickenWeapon.offset.x, 
                                                g_ChickenWeapon.offset.y, 
                                                g_ChickenWeapon.offset.z)
                              * Matrix_Rotate_X(g_ChickenWeapon.rotation.x)
                              * Matrix_Rotate_Y(g_ChickenWeapon.rotation.y)
                              * Matrix_Rotate_Z(g_ChickenWeapon.rotation.z)
                              * Matrix_Scale(g_ChickenWeapon.scale.x, 
                                            g_ChickenWeapon.scale.y, 
                                            g_ChickenWeapon.scale.z);
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(weaponModel));
        glUniform1i(g_object_id_uniform, MODEL_THOMPSON_GUN);
        
        // Desenha todas as partes da Thompson
        DrawVirtualObject("stock_Cube.010");
        DrawVirtualObject("M1A1_Cube.019");
        DrawVirtualObject("bolt_Cube.029");
        DrawVirtualObject("mag_Cube.030");
        DrawVirtualObject("magRelease_Plane.003");
        DrawVirtualObject("fireSelect_Cylinder.019");
        DrawVirtualObject("trigger_Cube.031");
        DrawVirtualObject("safety_Cylinder.025");
    }
}

void DrawAllTowers() {
    for (int i = 0; i < g_TowerCount; i++) {
        if (!g_Towers[i].active)
            continue;
        
        // Usa a posicao da fisica (com gravidade aplicada)
        DrawChickenWithWeapon(g_Towers[i].physics.position, true);
    }
}

bool CanPlaceTower(int gridX, int gridZ) {
    const int MAP_SIZE = 15;  // Tamanho do grid (mesmo valor de MAP_WIDTH e MAP_HEIGHT em main.cpp)
    if (gridX < 0 || gridX >= MAP_SIZE || gridZ < 0 || gridZ >= MAP_SIZE)
        return false;
    return g_MapGrid[gridZ][gridX] == CELL_EMPTY;
}
