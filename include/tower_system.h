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
    int gridX, gridZ;        // Posicao no grid
    bool active;             // Se a torre esta ativa
    PhysicsObject physics;   // Fisica da torre (gravidade, colisao)
    float attackRange;       // Alcance de ataque
    float attackDamage;      // Dano do ataque
    float attackSpeed;       // Velocidade de ataque (ataques por segundo)
    TowerType type;          // Tipo da torre (galinha ou beagle)
};

// ============================================================================
// VARIÁVEIS GLOBAIS DO SISTEMA DE TORRES
// ============================================================================

extern Tower g_Towers[MAX_TOWERS];
extern int g_TowerCount;
extern int g_SelectedTowerIndex;  // Índice da torre selecionada (-1 = nenhuma)

// Sistema de compra
extern bool g_ShowTowerMenu;      // Se o menu de compra está aberto
extern int g_MenuGridX;           // Grid X onde o menu foi aberto
extern int g_MenuGridZ;           // Grid Z onde o menu foi aberto

// ============================================================================
// FUNÇÕES DO SISTEMA DE TORRES
// ============================================================================

// Inicializa o sistema de torres
void InitializeTowers();

// Adiciona uma nova torre na posição especificada do grid
bool AddTower(int gridX, int gridZ, TowerType type);

// Atualiza a física de um objeto (gravidade e colisão)
void UpdatePhysics(PhysicsObject& obj, float deltaTime);

// Atualiza a física de todas as torres
void UpdateAllTowersPhysics(float deltaTime);

// Desenha uma galinha com arma anexada
void DrawChickenWithWeapon(glm::vec3 position, bool drawWeapon = true);

// Desenha um beagle com arma anexada
void DrawBeagleWithWeapon(glm::vec3 position, bool drawWeapon = true);

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

// Sistema de compra
void OpenTowerMenu(int gridX, int gridZ);
void CloseTowerMenu();
void BuyTower(TowerType type);
void ShowTowerMenuOnScreen();

#endif // TOWER_SYSTEM_H
