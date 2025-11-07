#ifndef TOWER_SYSTEM_H
#define TOWER_SYSTEM_H

#include <glm/glm.hpp>
#include "game_attributes.h"

// ============================================================================
// ESTRUTURAS DE FÍSICA E OBJETOS
// ============================================================================

struct PhysicsObject {
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    float radius;
    bool onGround;
};

// ============================================================================
// ESTRUTURA DE TORRE
// ============================================================================

struct Tower {
    int gridX, gridZ;        // Posicao no grid
    bool active;             // Se a torre esta ativa
    PhysicsObject physics;   // Fisica da torre (gravidade, colisao)
    float attackRange;       // Alcance de ataque
    float attackDamage;      // Dano do ataque
    float attackSpeed;       // Velocidade de ataque (ataques por segundo)
};

// ============================================================================
// CONSTANTES FÍSICAS
// ============================================================================

extern const float GRAVITY;
extern const float DAMPING;
extern const float MIN_VELOCITY;
extern const float CHICKEN_Y_OFFSET;

// ============================================================================
// VARIÁVEIS GLOBAIS DO SISTEMA DE TORRES
// ============================================================================

extern Tower g_Towers[MAX_TOWERS];
extern int g_TowerCount;
extern int g_SelectedTowerIndex;  // Índice da torre selecionada (-1 = nenhuma)

// ============================================================================
// FUNÇÕES DO SISTEMA DE TORRES
// ============================================================================

// Inicializa o sistema de torres
void InitializeTowers();

// Adiciona uma nova torre na posição especificada do grid
bool AddTower(int gridX, int gridZ);

// Atualiza a física de um objeto (gravidade e colisão)
void UpdatePhysics(PhysicsObject& obj, float deltaTime);

// Atualiza a física de todas as torres
void UpdateAllTowersPhysics(float deltaTime);

// Desenha uma galinha com arma anexada
void DrawChickenWithWeapon(glm::vec3 position, bool drawWeapon = true);

// Desenha todas as torres ativas
void DrawAllTowers();

// Verifica se pode colocar torre em uma posição
bool CanPlaceTower(int gridX, int gridZ);

// Seleciona torre mais próxima de uma posição no grid
int SelectTowerAtPosition(int gridX, int gridZ);

// Desenha círculo de alcance da torre selecionada
void DrawTowerRangeCircle();

// Exibe informações da torre selecionada
void ShowTowerInfo(int towerIndex);

#endif // TOWER_SYSTEM_H
