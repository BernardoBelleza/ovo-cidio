#include "tower_system.h"
#include "model_loader.h"
#include "hud.h"
#include <cstdio>
#include <cmath>
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

// Variáveis uniformes do shader (definidas em main.cpp)
extern GLint g_model_uniform;
extern GLint g_object_id_uniform;
extern GLuint g_GpuProgramID;

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
extern AttachedWeapon g_BeagleWeapon;

// ============================================================================
// CONSTANTES FÍSICAS
// ============================================================================

const float GRAVITY = -9.8f;
const float DAMPING = 0.6f;
const float MIN_VELOCITY = 0.05f;
const float CHICKEN_Y_OFFSET = -0.43f;
const float BEAGLE_Y_OFFSET = 0.15f;

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
        g_Towers[i].attackRange = 3.0f;   // 3 células de alcance
        g_Towers[i].attackDamage = 10.0f; // 10 de dano
        g_Towers[i].attackSpeed = 1.0f;   // 1 ataque por segundo
        g_Towers[i].type = TOWER_CHICKEN; // Tipo padrão
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
                           * Matrix_Scale(0.05f, 0.05f, 0.05f)
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

void DrawBeagleWithWeapon(glm::vec3 position, bool drawWeapon) {
    // Aplica offset Y para ajustar a base do modelo (ajuste conforme necessário)
    position.y += BEAGLE_Y_OFFSET;
    
    // Matriz de transformação do beagle
    glm::mat4 beagleModel = Matrix_Translate(position.x, position.y, position.z)
                          * Matrix_Scale(0.01f, 0.01f, 0.01f); // Escala (ajuste se necessário)
    
    glUniformMatrix4fv(g_model_uniform, 1, GL_FALSE, glm::value_ptr(beagleModel));
    glUniform1i(g_object_id_uniform, MODEL_BEAGLE_TOWER);
    DrawVirtualObject("Geo_Beagle"); // Nome correto do objeto no arquivo OBJ
    
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
        
        // Desenha a AK47
        DrawVirtualObject("Box003");
    }
}

void DrawAllTowers() {
    for (int i = 0; i < g_TowerCount; i++) {
        if (!g_Towers[i].active)
            continue;
        
        // Desenha a torre de acordo com seu tipo
        if (g_Towers[i].type == TOWER_CHICKEN) {
            DrawChickenWithWeapon(g_Towers[i].physics.position, true);
        } else if (g_Towers[i].type == TOWER_BEAGLE) {
            DrawBeagleWithWeapon(g_Towers[i].physics.position, true);
        }
    }
}

bool CanPlaceTower(int gridX, int gridZ) {
    const int MAP_SIZE = 15;  // Tamanho do grid (mesmo valor de MAP_WIDTH e MAP_HEIGHT em main.cpp)
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
        float angle1 = 2.0f * 3.14159f * i / segments;
        float angle2 = 2.0f * 3.14159f * (i + 1) / segments;
        
        glm::vec3 p1 = center + glm::vec3(range * cos(angle1), 0.0f, range * sin(angle1));
        glm::vec3 p2 = center + glm::vec3(range * cos(angle2), 0.0f, range * sin(angle2));
        
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
