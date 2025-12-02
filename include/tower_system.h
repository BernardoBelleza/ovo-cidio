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
    glm:: vec3 direction;
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
    TowerType type;          // Tipo da torre (galinha ou beagle)
    float cooldownTimer;              // Temporizador para controle de ataque
};

// ============================================================================
// VARIÁVEIS GLOBAIS DO SISTEMA DE TORRES
// ============================================================================

extern Tower g_Towers[MAX_TOWERS];
extern int g_TowerCount;
extern int g_SelectedTowerIndex;

// Sistema de compra
extern bool g_ShowTowerMenu;
extern int g_MenuGridX;
extern int g_MenuGridZ;

// ============================================================================
// FUNÇÕES DO SISTEMA DE TORRES
// ============================================================================

void InitializeTowers();

bool AddTower(int gridX, int gridZ, TowerType type);

void UpdatePhysics(PhysicsObject& obj, float deltaTime);

void UpdateAllTowersPhysics(float deltaTime);

void DrawChickenTower(glm::vec3 position, glm::vec3 direction);

void DrawBeagleTower(glm::vec3 position, glm::vec3 direction);

void DrawAllTowers();

bool CanPlaceTower(int gridX, int gridZ);

int SelectTowerAtPosition(int gridX, int gridZ);

void DrawTowerRangeCircle();

void ShowTowerInfo(int towerIndex);

// Busca a direçaoo da torre para o caminho mais próximo que ela encontrar
glm::vec3 GetDirectionToNearestPath(int originX, int originZ, float range);

// Busca a direção do inimigo mais adiantado dentro do alcance
void UpdateTowerTargeting(Tower& tower, float deltaTime);
void TowerShoot(Tower& tower);

// Sistema de compra
void OpenTowerMenu(int gridX, int gridZ);
void CloseTowerMenu();
void BuyTower(TowerType type);
void ShowTowerMenuOnScreen();


#endif // TOWER_SYSTEM_H
