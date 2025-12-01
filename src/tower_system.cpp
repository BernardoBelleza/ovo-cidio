#include "tower_system.h"
#include "hud.h"
#include "matrices.h"
#include "resource_loader.h"
#include "game_attributes.h"
#include <cstdio>
#include <cmath>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "projectile_system.h"
#include "enemy_system.h"

// ============================================================================
// DECLARAÇÕES EXTERNAS (funções e variáveis definidas em main.cpp)
// ============================================================================

// Funções de grid (definidas em main.cpp)
extern glm::vec3 GridToWorld(int gridX, int gridZ);
extern glm::ivec2 WorldToGrid(glm::vec3 worldPos);
extern float GetGroundHeight(int gridX, int gridZ);

// IDs dos modelos já estão definidos em resource_loader.h como #defines

// Estrutura de arma anexada (definida em main.cpp)
struct AttachedWeapon {
    glm::vec3 offset;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool enabled;
};
extern AttachedWeapon g_ChickenWeapon;
extern AttachedWeapon g_BeagleWeapon;

// ============================================================================
// VARIÁVEIS GLOBAIS DO SISTEMA DE TORRES
// ============================================================================

Tower g_Towers[MAX_TOWERS];
int g_TowerCount = 0;
int g_SelectedTowerIndex = -1;  // -1 = nenhuma torre selecionada

// Sistema de compra
bool g_ShowTowerMenu = false;
int g_MenuGridX = -1;
int g_MenuGridZ = -1;

// Lista de inimigos para ataques das torres
extern std::vector<Enemy> g_Enemies;

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
        g_Towers[i].physics.direction = glm::vec3(0.0f, 0.0f, 1.0f);
        g_Towers[i].physics.mass = 1.0f;
        g_Towers[i].physics.radius = 0.3f;
        g_Towers[i].physics.onGround = false;
        g_Towers[i].attackRange = 3.0f;
        g_Towers[i].attackDamage = 10.0f;
        g_Towers[i].attackSpeed = 1.0f;
        g_Towers[i].type = TOWER_CHICKEN;
        g_Towers[i].cooldownTimer = 0.0f;
    }



    g_TowerCount = 0;
    g_SelectedTowerIndex = -1;
    g_ShowTowerMenu = false;
    printf("[TORRE] Sistema de torres inicializado (max: %d)\n", MAX_TOWERS);
}

bool AddTower(int gridX, int gridZ, TowerType type) {
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
    
    // Pega a altura do terreno nessa posição
    float groundHeight = GetGroundHeight(gridX, gridZ);
    
    // Adiciona nova torre
    g_Towers[g_TowerCount].gridX = gridX;
    g_Towers[g_TowerCount].gridZ = gridZ;
    g_Towers[g_TowerCount].active = true;
    
    // Inicializa fisica da torre (spawna 3 blocos acima do chao para cair)
    g_Towers[g_TowerCount].physics.position = glm::vec3(worldPos.x, groundHeight + 3.0f, worldPos.z);
    g_Towers[g_TowerCount].physics.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    g_Towers[g_TowerCount].physics.mass = 1.0f;
    g_Towers[g_TowerCount].physics.radius = 0.3f;
    g_Towers[g_TowerCount].physics.onGround = false;
    
    // Inicializa atributos de combate
    g_Towers[g_TowerCount].attackRange = 3.0f;
    g_Towers[g_TowerCount].attackDamage = 10.0f;
    g_Towers[g_TowerCount].attackSpeed = 1.0f;
    g_Towers[g_TowerCount].type = type;
    g_Towers[g_TowerCount].cooldownTimer = 0.0f;
    

    // Inicializa direcao da torre
    g_Towers[g_TowerCount].physics.direction = GetDirectionToNearestPath(gridX, gridZ, g_Towers[g_TowerCount].attackRange);
    
    g_TowerCount++;
    

    const char* typeName = (type == TOWER_CHICKEN) ? "Galinha" : "Beagle";
    printf("[TORRE] Torre %s adicionada em (%d, %d) - Total: %d/%d\n", 
           typeName, gridX, gridZ, g_TowerCount, MAX_TOWERS);
    return true;
}

void UpdatePhysics(PhysicsObject& obj, float deltaTime) {
    if (obj.onGround && glm::abs(obj.velocity.y) < MIN_VELOCITY) {
        obj.velocity.y = 0.0f;
        return; // Objeto parou no chão
    }
    
    obj.velocity.y += GRAVITY * deltaTime;

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
            UpdateTowerTargeting(g_Towers[i], deltaTime);
        }
    }
}

void DrawChickenWithWeapon(glm::vec3 position, glm::vec3 direction, bool drawWeapon) {
    // Aplica offset Y para ajustar a base do modelo
    position.y += CHICKEN_Y_OFFSET;
    
    float angle = atan2f(direction.x, direction.z);

    // Matriz de transformação da galinha
    glm::mat4 chickenModel = Matrix_Translate(position.x, position.y, position.z)
                           * Matrix_Scale(0.05f, 0.05f, 0.05f)
                           * Matrix_Rotate_Y(angle);
    
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

void DrawBeagleWithWeapon(glm::vec3 position, glm::vec3 direction, bool drawWeapon) {
    position.y += BEAGLE_Y_OFFSET;

    float angle = atan2f(direction.x, direction.z);
    // Matriz de transformação do beagle
    glm::mat4 beagleModel = Matrix_Translate(position.x, position.y, position.z)
                          * Matrix_Scale(0.01f, 0.01f, 0.01f)
                          * Matrix_Rotate_Y(angle);
    
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(beagleModel));
    glUniform1i(g_object_id_uniform, MODEL_BEAGLE_TOWER);
    DrawVirtualObject("Geo_Beagle");
    
    // Desenha a AK47 se estiver habilitada
    if (drawWeapon && g_BeagleWeapon.enabled) {
        // Matriz de transformação da AK47 (aplicada à matriz do beagle)
        glm::mat4 weaponModel = beagleModel
                              * Matrix_Translate(g_BeagleWeapon.offset.x, 
                                                g_BeagleWeapon.offset.y, 
                                                g_BeagleWeapon.offset.z)
                              * Matrix_Rotate_X(g_BeagleWeapon.rotation.x)
                              * Matrix_Rotate_Y(g_BeagleWeapon.rotation.y)
                              * Matrix_Rotate_Z(g_BeagleWeapon.rotation.z)
                              * Matrix_Scale(g_BeagleWeapon.scale.x, 
                                            g_BeagleWeapon.scale.y, 
                                            g_BeagleWeapon.scale.z);
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(weaponModel));
        glUniform1i(g_object_id_uniform, MODEL_AK47);

        DrawVirtualObject("Box003");
    }
}

void DrawAllTowers() {
    for (int i = 0; i < g_TowerCount; i++) {
        if (!g_Towers[i].active)
            continue;
        
        // Desenha a torre de acordo com seu tipo
        if (g_Towers[i].type == TOWER_CHICKEN) {
            DrawChickenWithWeapon(g_Towers[i].physics.position, g_Towers[i].physics.direction, true);
        } else if (g_Towers[i].type == TOWER_BEAGLE) {
            DrawBeagleWithWeapon(g_Towers[i].physics.position, g_Towers[i].physics.direction, true);
        }
    }
}

bool CanPlaceTower(int gridX, int gridZ) {
    if (gridX < 0 || gridX >= MAP_SIZE || gridZ < 0 || gridZ >= MAP_SIZE)
        return false;
    return g_MapGrid[gridZ][gridX] == CELL_EMPTY;
}

// Seleciona torre exatamente na posição do grid clicado
int SelectTowerAtPosition(int gridX, int gridZ) {
    // Procura torre exatamente na posição clicada
    for (int i = 0; i < g_TowerCount; i++) {
        if (g_Towers[i].active && g_Towers[i].gridX == gridX && g_Towers[i].gridZ == gridZ) {
            return i;
        }
    }
    
    return -1; // Nenhuma torre encontrada nesta posição
}

// Desenha círculo de alcance ao redor da torre selecionada
void DrawTowerRangeCircle() {
    if (g_SelectedTowerIndex < 0 || g_SelectedTowerIndex >= g_TowerCount)
        return;
    
    if (!g_Towers[g_SelectedTowerIndex].active)
        return;
    
    Tower& tower = g_Towers[g_SelectedTowerIndex];
    glm::vec3 center = tower.physics.position;
    float range = tower.attackRange;
    int segments = 32;
    
    // Acessa variáveis globais do shader
    extern GLint g_model_uniform;
    extern GLint g_object_id_uniform;
    
    // Desenha várias linhas radiais formando um círculo
    for (int i = 0; i < segments; i++) {
        float angle1 = 2.0f * M_PI * i / segments;
        
        glm::vec3 p1 = center + glm::vec3(range * cos(angle1), 0.0f, range * sin(angle1));
        
        // Desenha pequenos planos/linhas conectando os pontos
        glm::mat4 model = Matrix_Translate(p1.x, center.y, p1.z)
                        * Matrix_Scale(0.05f, 0.05f, 0.05f);
        
        glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(g_object_id_uniform, 99); // ID para círculo amarelo
        DrawVirtualObject("the_plane");
    }
}

// Exibe informações da torre selecionada no console
void ShowTowerInfo(int towerIndex) {
    if (towerIndex < 0 || towerIndex >= g_TowerCount)
        return;
    
    if (!g_Towers[towerIndex].active)
        return;
    
    Tower& tower = g_Towers[towerIndex];
    
    printf("\n========== TORRE #%d ==========\n", towerIndex + 1);
    printf("Posicao Grid: (%d, %d)\n", tower.gridX, tower.gridZ);
    printf("Posicao World: (%.2f, %.2f, %.2f)\n", 
           tower.physics.position.x, 
           tower.physics.position.y, 
           tower.physics.position.z);
    printf("Alcance: %.1f celulas\n", tower.attackRange);
    printf("Dano: %.1f\n", tower.attackDamage);
    printf("Velocidade Ataque: %.1f atk/s\n", tower.attackSpeed);
    printf("No chao: %s\n", tower.physics.onGround ? "Sim" : "Nao");
    printf("Tipo: %s\n", tower.type == TOWER_CHICKEN ? "Galinha" : "Beagle");
    printf("===============================\n\n");
}

// ============================================================================
// SISTEMA DE COMPRA DE TORRES
// ============================================================================

void OpenTowerMenu(int gridX, int gridZ) {
    if (!CanPlaceTower(gridX, gridZ)) {
        AddConsoleMessage("[ERRO] Nao pode colocar torre aqui!");
        return;
    }
    
    g_ShowTowerMenu = true;
    g_MenuGridX = gridX;
    g_MenuGridZ = gridZ;
    
    AddConsoleMessage("=== MENU DE TORRES ===");
    AddConsoleMessage("[1] Galinha ($100) | [2] Beagle ($150)");
    AddConsoleMessage("Pressione 1 ou 2 para comprar, ESC para cancelar");
}

void CloseTowerMenu() {
    g_ShowTowerMenu = false;
    g_MenuGridX = -1;
    g_MenuGridZ = -1;
}

void BuyTower(TowerType type) {
    if (!g_ShowTowerMenu) {
        AddConsoleMessage("[ERRO] Menu nao esta aberto!");
        return;
    }
    
    const char* typeName = (type == TOWER_CHICKEN) ? "Galinha" : "Beagle";
    int cost = (type == TOWER_CHICKEN) ? 100 : 150;
    
    // Verificar se tem dinheiro suficiente
    if (!SpendMoney(cost)) {
        AddConsoleMessage("[ERRO] Dinheiro insuficiente!");
        return;
    }
    
    // Tentar adicionar a torre
    if (AddTower(g_MenuGridX, g_MenuGridZ, type)) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Torre %s comprada! (-$%d)", typeName, cost);
        AddConsoleMessage(msg);
        CloseTowerMenu();
    } else {
        // Devolver o dinheiro se falhou
        AddMoney(cost);
        AddConsoleMessage("[ERRO] Falha ao comprar torre!");
    }
}

void ShowTowerMenuOnScreen() {
    // Esta função será chamada no loop de renderização
    // Por enquanto, o menu é mostrado no console
    // No futuro, pode ser implementado um menu visual na tela
}

glm::vec3 GetDirectionToNearestPath(int originX, int originZ, float range) {
    int foundX = -1;
    int foundZ = -1;
    bool found = false;
    int radius;

    int searchRadiusLimit = (int) ceil(range); 

    for (radius = 1; radius <= searchRadiusLimit ; radius++) {
        for (int x = originX - radius; x <= originX + radius ; x++) {
            for (int z = originZ - radius; z <= originZ + radius ; z++) {
                
                // Se não estiver na borda do quadrado, pula
                if (abs(x - originX) != radius && abs(z - originZ) != radius)
                    continue;
                // Se estiver no grid e for uma célula path, achamos
                if (x >= 0 && x < MAP_WIDTH && z >= 0 && z < MAP_HEIGHT) {
                    if (g_MapGrid[z][x] == CELL_PATH) {
                        foundX = x;
                        foundZ = z;
                        found = true;
                        break; 
                    }
                }
            }
            if (found) break;
        }
        if (found) break;
    }

    if (found) {
        glm::vec3 targetPos = GridToWorld(foundX, foundZ);
        glm::vec3 towerPos = g_Towers[g_TowerCount].physics.position;
        
        return glm::normalize(glm::vec3(targetPos.x - towerPos.x, 0.0f, targetPos.z - towerPos.z));

    } else {
        // Não encontramos nada, olha para frente
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }
}

void UpdateTowerTargeting(Tower& tower, float deltaTime) {
    
    if (tower.cooldownTimer > 0.0f) {
        tower.cooldownTimer -= deltaTime;
    }

    Enemy* target = nullptr;
    
    // Variável para guardar a posição do mais avançado
    float maxProgress = -1.0f; 

    glm::vec3 towerPos = glm::vec3(tower.physics.position.x, tower.physics.position.y, tower.physics.position.z);

    for (size_t i = 0; i < g_Enemies.size(); i++) {
        Enemy& enemy = g_Enemies[i];

        // Se o inimigo não existir mais ou estiver morto, ignoramos
        if (!enemy.active || enemy.health <= 0.0f) 
            continue;

        glm::vec3 enemyPos = glm::vec3(enemy.position.x, enemy.position.y, enemy.position.z);
        float dist = glm::distance(towerPos, enemyPos);

        if (dist <= tower.attackRange) {
            // Soma do progresso no caminho pelo caminho total e caminho parcial
            float totalProgress = (float)enemy.currentPathIndex + enemy.pathProgress;

            if (totalProgress > maxProgress) {
                maxProgress = totalProgress;
                target = &enemy;
            }
        }
    }

    // Alterando direção da mira (se houver inimigo)
    if (target != nullptr) {
        // Mantemos a direção y para o tiro subir/descer nos inimigos voadores ou pequenos

        const EnemyRenderInfo& renderInfo = GetEnemyRenderInfo(target->type);
        glm::vec3 targetRenderPos = glm::vec3(
            target->position.x,
            target->position.y + renderInfo.yOffset,
            target->position.z
        );
        
        glm::vec3 dirToEnemy = targetRenderPos - tower.physics.position;

        // Eles não devem estar na mesma posição, mas garantimos que não ocorrerá divisão por 0        
        if (glm::length(dirToEnemy) > 0.01f) {
            tower.physics.direction = glm::normalize(dirToEnemy);
        }
        TowerShoot(tower);
    } 
}


void TowerShoot(Tower& tower) {
    if (tower.cooldownTimer > 0.0f) return;

    glm::vec3 spawnPos = tower.physics.position; // Posicao da torre é a posição inicial do projetil
    spawnPos.y += 0.5f; 


    SpawnProjectile(spawnPos, tower.physics.direction, tower.attackDamage);

    // Reset timer
    if (tower.attackSpeed > 0.0f) {
        tower.cooldownTimer = 1.0f / tower.attackSpeed;
    } else {
        tower.cooldownTimer = 1.0f;
    }
}


