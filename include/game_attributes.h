#ifndef GAME_ATTRIBUTES_H
#define GAME_ATTRIBUTES_H

// ============================================================================
// ATRIBUTOS DE TORRES
// ============================================================================

struct TowerAttributes {
    float attackDamage;      // Dano por ataque
    float attackRange;       // Alcance de ataque (em unidades de grid)
    float attackSpeed;       // Ataques por segundo
    int buildCost;           // Custo de construcao
};

// Torres disponiveis
const TowerAttributes CHICKEN_TOWER = {
    10.0f,  // attackDamage
    3.0f,   // attackRange
    1.5f,   // attackSpeed
    50      // buildCost
};

// ============================================================================
// ATRIBUTOS DE INIMIGOS
// ============================================================================

struct EnemyAttributes {
    float maxHealth;         // Vida maxima
    float moveSpeed;         // Velocidade de movimento
    int goldReward;          // Ouro ao morrer
    int damageToBase;        // Dano causado a base
};

// Inimigos disponiveis
const EnemyAttributes BASIC_ENEMY = {
    50.0f,  // maxHealth
    2.0f,   // moveSpeed
    10,     // goldReward
    1       // damageToBase
};

const EnemyAttributes FAST_ENEMY = {
    30.0f,  // maxHealth
    4.0f,   // moveSpeed
    15,     // goldReward
    1       // damageToBase
};

const EnemyAttributes TANK_ENEMY = {
    150.0f, // maxHealth
    1.0f,   // moveSpeed
    30,     // goldReward
    3       // damageToBase
};

// ============================================================================
// ATRIBUTOS DE PROJETEIS
// ============================================================================

struct ProjectileAttributes {
    float speed;             // Velocidade do projetil
    float damage;            // Dano causado
    float maxDistance;       // Distancia maxima antes de desaparecer
};

// Projeteis disponiveis
const ProjectileAttributes BULLET = {
    15.0f,  // speed
    10.0f,  // damage
    10.0f   // maxDistance
};

// ============================================================================
// CONFIGURACOES GLOBAIS DO JOGO
// ============================================================================

const int PLAYER_STARTING_GOLD = 200;
const int PLAYER_STARTING_LIVES = 20;
const int MAX_TOWERS = 20;
const int MAX_ENEMIES = 50;
const int MAX_PROJECTILES = 100;

#endif // GAME_ATTRIBUTES_H
