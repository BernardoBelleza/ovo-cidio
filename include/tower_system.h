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

// Tipos de torres disponíveis
enum TowerType {
    TOWER_CHICKEN = 0,
    TOWER_BEAGLE = 1,
    TOWER_TYPE_COUNT = 2
};

struct Tower {
    int gridX, gridZ;
    bool active;
    PhysicsObject physics;
    float attackRange;
    float attackDamage;
    float attackSpeed;
    TowerType type;
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

void DrawChickenTower(glm::vec3 position);

void DrawBeagleTower(glm::vec3 position);

void DrawAllTowers();

bool CanPlaceTower(int gridX, int gridZ);

int SelectTowerAtPosition(int gridX, int gridZ);

void DrawTowerRangeCircle();

void ShowTowerInfo(int towerIndex);

// Sistema de compra
void OpenTowerMenu(int gridX, int gridZ);
void CloseTowerMenu();
void BuyTower(TowerType type);
void ShowTowerMenuOnScreen();

#endif // TOWER_SYSTEM_H
