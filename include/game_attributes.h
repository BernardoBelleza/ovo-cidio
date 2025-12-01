#ifndef GAME_ATTRIBUTES_H
#define GAME_ATTRIBUTES_H

// Constantes matemáticas
#define M_PI   3.141592f
#define M_PI_2 1.570781f

// Constantes físicas
const float GRAVITY = -9.8f;
const float DAMPING = 0.6f;
const float MIN_VELOCITY = 0.05f;
const float CHICKEN_Y_OFFSET = -0.35f;
const float BEAGLE_Y_OFFSET = -0.05f;
const float BEAGLE_Z_OFFSET = 0.2f;

// Mapa
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 15;
const int MAP_SIZE = 15;

// Planos do grid
#define CELL_EMPTY_PLANE   10
#define CELL_PATH_PLANE    11
#define CELL_BLOCKED_PLANE 12
#define CELL_BASE_PLANE    13
#define CELL_START_PLANE   14

// Torres
#define MODEL_CHICKEN_TOWER  20
#define MODEL_BEAGLE_TOWER   21
#define MODEL_CHICKEN_COOP   40

// Armas
#define MODEL_THOMPSON_GUN   25
#define MODEL_AK47           26

// Inimigos
#define MODEL_HAWK           30
#define MODEL_FOX            31
#define MODEL_WOLF           32
#define MODEL_RAT            33


#define MODEL_EGG 50

// Círculo de alcance da torre
#define TOWER_RANGE_CIRCLE  99

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
    CELL_BASE = 3,
    CELL_START = 4
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

enum EnemyType {
    ENEMY_WOLF = 0,
    ENEMY_HAWK,
    ENEMY_FOX,
    ENEMY_RAT
};

struct Enemy {
    EnemyType type;
    glm::vec3 position;
    glm::vec3 direction;
    int currentPathIndex;
    float health;
    float maxHealth;
    float moveSpeed;
    bool active;
    float pathProgress;
};

struct EnemyAttributes {
    float maxHealth;
    float moveSpeed;
    int goldReward;
    int damageToBase;
};

// Inimigos disponiveis
const EnemyAttributes WOLF_ATTRIBUTES = {
    100.0f,
    1.5f,
    20,
    2
};

const EnemyAttributes HAWK_ATTRIBUTES = {
    50.0f,
    2.5f,
    15,
    1
};

const EnemyAttributes FOX_ATTRIBUTES = {
    75.0f,
    2.0f,
    25,
    1
};

const EnemyAttributes RAT_ATTRIBUTES = {
    20.0f,
    3.0f,
    10,
    1
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

// ============================================================================
// CONFIGURAÇÕES DE RENDERIZAÇÃO DE INIMIGOS
// ============================================================================

struct EnemyRenderInfo {
    float scaleX, scaleY, scaleZ;
    float yOffset;
    const char* meshName;
    float collisionRadius; // Adicione esta linha

};

const EnemyRenderInfo WOLF_RENDER = {
    0.005f, 0.005f, 0.005f,
    0.4f,
    "Geo_Wolf",
    0.5f
};

const EnemyRenderInfo HAWK_RENDER = {
    0.15f, 0.15f, 0.15f,
    2.0f,
    "FerruginousHawk_Mesh",
    0.3f
};

const EnemyRenderInfo FOX_RENDER = {
    0.1f, 0.1f, 0.1f,
    0.0f,
    "Fox_Mesh",
    0.4f
};

const EnemyRenderInfo RAT_RENDER = {
    0.1f, 0.1f, 0.1f,
    0.0f,
    "KangarooRats_Mesh",
    0.3f
};

const float ENEMY_BEZIER_SMOOTHNESS = 0.3f;
const float MIN_SEGMENT_LENGTH = 0.001f;

#endif // GAME_ATTRIBUTES_H
