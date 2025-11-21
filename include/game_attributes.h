#ifndef GAME_ATTRIBUTES_H
#define GAME_ATTRIBUTES_H

// Constantes matemáticas
#define M_PI   3.141592f
#define M_PI_2 1.570781f

// Constantes físicas
const float GRAVITY = -9.8f;
const float DAMPING = 0.6f;
const float MIN_VELOCITY = 0.05f;
const float CHICKEN_Y_OFFSET = -0.43f;
const float BEAGLE_Y_OFFSET = 0.15f;

// Mapa
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 15;
const int MAP_SIZE = 15;

// Planos do grid
#define CELL_EMPTY_PLANE   10
#define CELL_PATH_PLANE    11
#define CELL_BLOCKED_PLANE 12
#define CELL_BASE_PLANE    13

// Torres
#define MODEL_CHICKEN_TOWER  20
#define MODEL_BEAGLE_TOWER   21
#define MODEL_CHICKEN_COOP   40

// Armas
#define MODEL_THOMPSON_GUN   25
#define MODEL_AK47           26

// Configurações do jogo
const int PLAYER_STARTING_GOLD = 200;
const int PLAYER_STARTING_LIVES = 20;
const int MAX_TOWERS = 20;
const int MAX_ENEMIES = 50;
const int MAX_PROJECTILES = 100;
const int PLAYER_MONEY_START = 10001;

// Tipos de células
enum CellType {
    CELL_EMPTY = 0,
    CELL_PATH = 1,
    CELL_BLOCKED = 2,
    CELL_BASE = 3
};

// Grid do mapa
extern CellType g_MapGrid[MAP_HEIGHT][MAP_WIDTH];

// ============================================================================
// ATRIBUTOS DE TORRES
// ============================================================================

struct TowerAttributes {
    float attackDamage;
    float attackRange;
    float attackSpeed;
    int buildCost;
};

// Torres disponiveis
const TowerAttributes CHICKEN_TOWER = {
    10.0f,
    3.0f,
    1.5f,
    50
};

// ============================================================================
// ATRIBUTOS DE INIMIGOS
// ============================================================================

struct EnemyAttributes {
    float maxHealth;
    float moveSpeed;
    int goldReward;
    int damageToBase;
};

// Inimigos disponiveis
const EnemyAttributes BASIC_ENEMY = {
    50.0f,
    2.0f,
    10,
    1
};

const EnemyAttributes FAST_ENEMY = {
    30.0f,
    4.0f,
    15,
    1
};

const EnemyAttributes TANK_ENEMY = {
    150.0f,
    1.0f,
    30,
    3
};

// ============================================================================
// ATRIBUTOS DE PROJETEIS
// ============================================================================

struct ProjectileAttributes {
    float speed;
    float damage;
    float maxDistance;
};

// Projeteis disponiveis
const ProjectileAttributes BULLET = {
    15.0f,
    10.0f,
    10.0f
};

#endif // GAME_ATTRIBUTES_H
